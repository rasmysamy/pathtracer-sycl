//
// Created by sysgen on 4/7/21.
//

#ifndef PATHTRACER_SYCL_KDTREEMESH_H
#define PATHTRACER_SYCL_KDTREEMESH_H

#include "Triangle.h"
#include "AABB.h"
//#include "Material.h"
#include "Mesh.h"
#include <stack>


//namespace std {
//    void __throw_length_error(char const*) {
//    }
//    void __throw_bad_allocv(char const*){
//    }
//}//This bypasses linker error when using std::stack on GPU

struct materialBase{
    sc::float3 attr_1 = {50,50,50}; //Generally color, can also be emissivity
    float attr_2 = 0; //This is either index of refraction, or roughness.
    int material = 0; //0 = Diffuse, 1 = Emissive, 2=Glossy

    materialBase(const sc::float3 &attr1, float attr2, int material) : attr_1(attr1), attr_2(attr2),
                                                                                  material(material) {}
};
struct kdTreeNode{
    kdTreeNode()=default;

    kdTreeNode *nodes; //This is technically an array.
    AABB *bound;
    bool isLeaf;
    Triangle *triangles; //Last two only for leaves
    int nmPrimitives = 0;

    kdTreeNode(const kdTreeNode &a, const kdTreeNode &b, int dimSplit, AABB *bounds, int depth, sc::queue q) :
                                                                                             isLeaf(false),
                                                                                             bound(bounds)
                                                                                             {
        nodes = sc::malloc_shared<kdTreeNode>(2, q);
        nodes[0] = a;
        nodes[1] = b;
    }


    kdTreeNode(AABB *bounds, Triangle *tri, int nmPrimitives, sc::queue q) : bound(bounds), isLeaf(true), nmPrimitives(nmPrimitives){
        triangles = sc::malloc_shared<Triangle>(nmPrimitives, q);
        std::copy(tri, tri+nmPrimitives, triangles);
    }

//    ~kdTreeNode() {
//        delete triangles;
//    }

    void split(sc::queue q){
        sc::float3 bBoxSize = bound->getSize();
        float maxDim = sc::max(sc::max(bBoxSize.x(), bBoxSize.y()), bBoxSize.z());
        int c_dimSplit = 0;
        if(maxDim == bBoxSize.x())
            c_dimSplit = 0;
        else if (maxDim == bBoxSize.y())
            c_dimSplit = 1;
        else
            c_dimSplit = 2;
        //We now have to find the median along that axis.

        std::sort(triangles, triangles+nmPrimitives,
                  [&split = std::as_const(c_dimSplit)](Triangle a, Triangle b){
            return a.getCenter()[split] > b.getCenter()[split];
        }); //This is not strictly optimal, we could presort with min and max in all dimensions beforethen, but that adds a lot of complexity.

        //Now that the array is sorted alongside the split dimension :D
        //We can thus easily find the median and split alongside that.

        Triangle medianElement = triangles[nmPrimitives/2];
        AABB *boxes = sc::malloc_shared<AABB>(2, q);
        boxes[0] = *bound;
        boxes[1] = *bound;

        float splitCoord = medianElement.getCenter()[c_dimSplit];
        boxes[0].bounds[1][c_dimSplit] = splitCoord;
        boxes[1].bounds[0][c_dimSplit] = splitCoord;
        //Now that we have all of the AABBs, we have to populate them with triangles.
        //We could use the AABB-tri intersection, but we can massivelly optimize because we know that the triangles
        //already intersect in all but the split axis.

        std::vector<Triangle> aTris = std::vector<Triangle>();
        std::vector<Triangle> bTris = std::vector<Triangle>();
        //insertion has a linear complexity when reallocation is necessary. We can preallocate a reasonable size to
        //effectively eliminate the need to do reallocations.

        for (int i = 0; i < nmPrimitives; ++i) {
            Triangle elem = triangles[i];
            float primAxisMin = sc::min(sc::min(elem.v0[c_dimSplit], elem.v1[c_dimSplit]), elem.v2[c_dimSplit]);
            float primAxisMax = sc::max(sc::max(elem.v0[c_dimSplit], elem.v1[c_dimSplit]), elem.v2[c_dimSplit]);
            if(primAxisMin < splitCoord) {
                Triangle t = Triangle(elem);
                aTris.push_back(t);
            }
            if(primAxisMax > splitCoord) {
                Triangle t = Triangle(elem);
                bTris.push_back(t);
            }
        }
        //Now that we have the element lists, we can set this node to be a parent node and free up memory, as well as
        //initialize child nodes
        Triangle *aArr = sc::malloc_shared<Triangle>(aTris.size(), q);
        Triangle *bArr = sc::malloc_shared<Triangle>(bTris.size(), q);
        std::copy(aTris.begin(), aTris.end(), aArr);
        std::copy(bTris.begin(), bTris.end(), bArr);

        nodes = sc::malloc_shared<kdTreeNode>(2, q);
        nodes[0] = kdTreeNode(boxes, aArr, aTris.size(), q);
        nodes[1] = kdTreeNode(boxes+1, bArr, bTris.size(), q);

        isLeaf = false;
//        delete triangles;
    }

//    void finalizeLeafSharedMemory(sc::queue q){ //Prepare the triangle array/pointer to be replicated on the SYCL compute device
//        if(!isLeaf) {
//            kdTreeNode *nT = nodes;
//            nodes = sc::malloc_shared<kdTreeNode>(2, q);
//            nodes[0] = nT[0];
//            nodes[1] = nT[1];
//            return;
//        }
//        auto *sharedData = sc::malloc_shared<Triangle>(nmPrimitives, q);
//        for (int i = 0; i < nmPrimitives; ++i) {
//            sharedData[i] = triangles[i];
//        }
//        triangles = sharedData;
//    }

    material::intersectReturn mRayIntersect(const Ray& r, Triangle* ref, const material::intersectReturn& lastIntersect, const materialBase& mat) const{
        float minDist = FLT_MAX;
        material::intersectReturn t = material::intersectReturn();
        for (int i = 0; i < nmPrimitives; ++i) {
            material::intersectReturn ret = triangles[i].rayIntersect(r, mat.material, mat.attr_1, mat.attr_2);
            if(ret.intersect && ret.intersectDistance<minDist && (int)((triangles + i) - ref)!=lastIntersect.stackPos){
                minDist = ret.intersectDistance;
                ret.stackPos=(int)((triangles + i) - ref);//This is basically a cheap way of getting a hash
                ret.objType=1;
                t=ret;
            }
        }
        t.material = mat.material;
        t.attr_1 = mat.attr_1;
        t.attr_2 = mat.attr_2;
        return t;
    }
};

class kdTreeMesh{
    int nbLeafNodes;
    int maxDepth;
    materialBase m;
public:
    kdTreeNode parentNode;
    kdTreeMesh(std::vector<Triangle> t, AABB bounds, int mDepth, int minPoints, sc::queue q, materialBase mat) : m(mat){
        Triangle *tArr = sc::malloc_shared<Triangle>(t.size(), q);
        std::copy(t.begin(), t.end(), tArr);
        AABB *b = sc::malloc_shared<AABB>(1, q);
        *b = bounds;
        parentNode = kdTreeNode(b, tArr, t.size(), q);
        parentNode.nmPrimitives = t.size();
        nbLeafNodes = 1;
        maxDepth = mDepth;
        constructKdTree(mDepth, minPoints, q);
    }
    void constructKdTree(int maxDepth, int minPoints, sc::queue q){
        std::vector<kdTreeNode*> lNodes = std::vector<kdTreeNode*>();
        lNodes.push_back(&parentNode);
        bool hasUpdated = true;
        int i=0;
        while(hasUpdated){
            std::vector<kdTreeNode*> newLNodes = std::vector<kdTreeNode*>();
            hasUpdated = false;
            if(i>=maxDepth)
                break;
            for(kdTreeNode* n : lNodes){
                if(n->nmPrimitives<=2*minPoints) {
                    newLNodes.emplace_back(n);
                    continue;
                }
                n->split(q);
                newLNodes.push_back(&(n->nodes[0]));
                newLNodes.push_back(&(n->nodes[1]));
                hasUpdated = true;
            }
            i++;
            lNodes = newLNodes;
        }
    }
    struct visitFlag{
        bool seenFirst=false;
        bool seenSecond=false;
    };

    material::intersectReturn mRayIntersect(const Ray &r, const material::intersectReturn& lastIntersect) const{
        //This code is a stub for kdTree-ray intersection. It misses collision due to missing AABB collision point data
        kdTreeNode currentNode = parentNode;
        kdTreeNode visitedList[32];
        kdTreeMesh::visitFlag vFlags[32];
        material::intersectReturn ret;
        int currentDepth = 0;
        if(!(currentNode.bound->bRayIntersect(r))) {
            return ret;
        }
        if(parentNode.isLeaf){
            return parentNode.mRayIntersect(r, parentNode.triangles, lastIntersect, m);
        }
        while(true){
            visitedList[currentDepth] = kdTreeNode(currentNode);
            if(currentNode.isLeaf){
                auto t = currentNode.mRayIntersect(r, parentNode.triangles, lastIntersect, m);
                if(t.intersect) {
                    return t;
                }
                //no intersection, we must backtrack
                --currentDepth;
                currentNode = visitedList[currentDepth];
                continue;
            }
            //So, this is not a leaf. We check if there is a non-visited node.
            else if(vFlags[currentDepth].seenFirst && vFlags[currentDepth].seenSecond){
                //Both nodes are visited. We check if we are a at depth zero. If so, there is no intersection.
                if(currentDepth==0) {
                    return ret;
                }

                vFlags[currentDepth] = visitFlag(); //reset visitation flags
                --currentDepth; //backtrack to previous level
                currentNode = visitedList[currentDepth];
                continue;
            }
            else if(!(vFlags[currentDepth].seenFirst) && currentNode.nodes[0].bound->fRayIntersect(r)!=-1
                &&!(vFlags[currentDepth].seenSecond) && currentNode.nodes[1].bound->fRayIntersect(r)!=-1){
                //In this case, the ray intersects with both. We have to investigate only the closest node.
                if(currentNode.nodes[0].bound->fRayIntersect(r) < currentNode.nodes[1].bound->fRayIntersect(r))
                    goto checkingFirst;
                goto checkingSecond; //To avoid using a function call and increasing stack depth, we use goto here.
            }
            else if(!(vFlags[currentDepth].seenFirst) && currentNode.nodes[0].bound->fRayIntersect(r)!=-1){//If the first node isn't seen, we switch to it
                checkingFirst:
                vFlags[currentDepth].seenFirst=true;
                currentNode = (currentNode.nodes[0]);
                currentDepth++;
                continue;
            }
            else if(!(vFlags[currentDepth].seenSecond) && currentNode.nodes[1].bound->fRayIntersect(r)!=-1){
                checkingSecond:
                vFlags[currentDepth].seenSecond=true;
                currentNode = (currentNode.nodes[1]);
                currentDepth++;
                continue;
            }
            //No intersection at all also means we must backtrack
            else if(currentDepth==0) {
                return ret;
            }
            vFlags[currentDepth] = visitFlag(); //reset visitation flags
            --currentDepth; //backtrack to previous level
            currentNode = visitedList[currentDepth];
        }
    }

    kdTreeNode getParentNode(){
        return parentNode;
    }

    const materialBase &getM() const {
        return m;
    }

};
#endif //PATHTRACER_SYCL_KDTREEMESH_H
