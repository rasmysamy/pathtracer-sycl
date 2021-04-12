//
// Created by sysgen on 3/30/21.
//

#ifndef PATHTRACER_SYCL_MESH_H
#define PATHTRACER_SYCL_MESH_H

#pragma once

#include <utility>

#include "Triangle.h"
#include "AABB.h"

#include "tiny_obj_loader.h"

using material::MATERIALS;

AABB generateBoundingVolume(std::vector<Triangle> &tVec) {
    sc::float3 min, max;
    for(Triangle t : tVec){
        sc::float3 a, b, c;
        a = t.v0;
        b = t.v1;
        c = t.v2;
        //Now we set the maximums and minimums
        {
            max.x() = sc::max(max.x(), a.x());
            max.x() = sc::max(max.x(), b.x());
            max.x() = sc::max(max.x(), c.x());

            max.y() = sc::max(max.y(), a.y());
            max.y() = sc::max(max.y(), b.y());
            max.y() = sc::max(max.y(), c.y());

            max.z() = sc::max(max.z(), a.z());
            max.z() = sc::max(max.z(), b.z());
            max.z() = sc::max(max.z(), c.z());

            min.x() = sc::min(min.x(), a.x());
            min.x() = sc::min(min.x(), b.x());
            min.x() = sc::min(min.x(), c.x());

            min.y() = sc::min(min.y(), a.y());
            min.y() = sc::min(min.y(), b.y());
            min.y() = sc::min(min.y(), c.y());

            min.z() = sc::min(min.z(), a.z());
            min.z() = sc::min(min.z(), b.z());
            min.z() = sc::min(min.z(), c.z());
        }
    }
    //Now that we have the two extremes points, we can generate the bounding boxes.
    return AABB(min, max);
}

bool vectorEquals(sc::float3 a, sc::float3 b){
    return a[0]==a[0] && a[1]==b[1] && a[2]==b[2];
}

std::vector<Triangle> readMesh(AABB &boundingVolume, std::string objPath){
    auto triangleVec = std::vector<Triangle>();
    tinyobj::ObjReaderConfig reader_config;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(objPath, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

// Loop over shapes
    for (const auto & shape : shapes) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            auto fv = size_t(shape.mesh.num_face_vertices[f]);

            size_t v = 0;
            tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
            tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
            tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
            tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

            tinyobj::real_t vnx = attrib.normals[3*size_t(idx.normal_index)+0];
            tinyobj::real_t vny = attrib.normals[3*size_t(idx.normal_index)+1];
            tinyobj::real_t vnz = attrib.normals[3*size_t(idx.normal_index)+2];
            sc::float3 A = {vx, vz, -vy};//This corrects for Blender's coordinate system (Z-Up).
            sc::float3 An = {vnx, vnz, -vny};
            v++;
            idx = shape.mesh.indices[index_offset + v];
            vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
            vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
            vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

            vnx = attrib.normals[3*size_t(idx.normal_index)+0];
            vny = attrib.normals[3*size_t(idx.normal_index)+1];
            vnz = attrib.normals[3*size_t(idx.normal_index)+2];
            sc::float3 B = {vx, vz, -vy};
            sc::float3 Bn = {vnx, vnz, -vny};
            v++;
            idx = shape.mesh.indices[index_offset + v];
            vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
            vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
            vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

            vnx = attrib.normals[3*size_t(idx.normal_index)+0];
            vny = attrib.normals[3*size_t(idx.normal_index)+1];
            vnz = attrib.normals[3*size_t(idx.normal_index)+2];
            sc::float3 C = {vx, vz, -vy};
            sc::float3 Cn = {vnx, vnz, -vny};
            v++;

            bool isFlat=(vectorEquals(Cn, Bn) && vectorEquals(An, Bn)); //right now we don't support smooth normals
            if(isFlat){
                An = (An+Cn+Bn)/3;
            }
            triangleVec.emplace_back(Triangle(A,B,C,An,An,An, true));

            index_offset += fv;
        }
    }

    boundingVolume = generateBoundingVolume(triangleVec);
    return triangleVec;
}

//class MeshStage{
//public:
//    int material;
//    sc::float3 attr_1;
//    AABB boundingVolume;
//    int offset;
//    int size;
//
//    MeshStage(std::string objPath, std::vector<Triangle> *tVec, int mat, sc::float3 color){
//        material = mat;
//        offset = tVec->size();
//
//        tinyobj::ObjReaderConfig reader_config;
//
//        tinyobj::ObjReader reader;
//
//        if (!reader.ParseFromFile(objPath, reader_config)) {
//            if (!reader.Error().empty()) {
//                std::cerr << "TinyObjReader: " << reader.Error();
//            }
//            exit(1);
//        }
//
//        if (!reader.Warning().empty()) {
//            std::cout << "TinyObjReader: " << reader.Warning();
//        }
//
//        auto& attrib = reader.GetAttrib();
//        auto& shapes = reader.GetShapes();
//
//
//// Loop over shapes
//        for (const auto & shape : shapes) {
//            // Loop over faces(polygon)
//            size_t index_offset = 0;
//            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
//                auto fv = size_t(shape.mesh.num_face_vertices[f]);
//
//                size_t v = 0;
//                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
//                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
//                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
//                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
//
//                tinyobj::real_t vnx = attrib.normals[3*size_t(idx.vertex_index)+0];
//                tinyobj::real_t vny = attrib.normals[3*size_t(idx.vertex_index)+1];
//                tinyobj::real_t vnz = attrib.normals[3*size_t(idx.vertex_index)+2];
//                sc::float3 A = {vx, vz, -vy};//This corrects for Blender's coordinate system (Z-Up).
//                sc::float3 An = {vnx, vnz, -vny};
//                v++;
//                idx = shape.mesh.indices[index_offset + v];
//                vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
//                vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
//                vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
//
//                vnx = attrib.normals[3*size_t(idx.vertex_index)+0];
//                vny = attrib.normals[3*size_t(idx.vertex_index)+1];
//                vnz = attrib.normals[3*size_t(idx.vertex_index)+2];
//                sc::float3 B = {vx, vz, -vy};
//                sc::float3 Bn = {vnx, vnz, -vny};
//                v++;
//                idx = shape.mesh.indices[index_offset + v];
//                vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
//                vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
//                vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
//
//                vnx = attrib.normals[3*size_t(idx.vertex_index)+0];
//                vny = attrib.normals[3*size_t(idx.vertex_index)+1];
//                vnz = attrib.normals[3*size_t(idx.vertex_index)+2];
//                sc::float3 C = {vx, vz, -vy};
//                sc::float3 Cn = {vnx, vnz, -vny};
//                v++;
//
//                tVec->emplace_back(Triangle(A,B,C, color, material));
//
//                index_offset += fv;
//
//                // per-face material
//                shape.mesh.material_ids[f];
//            }
//        }
//
//        size = tVec->size()-offset;
//
//        boundingVolume = generateBoundingVolume(tVec);
//    }
//};
//
//class Mesh {
//    int material;
//    sc::float3 attr_1;
//    AABB boundingVolume;
//    Triangle *tris;
//    int offset;
//    int size;
//public:
//    Mesh()= default;
//    Triangle getTriangle(int n) const {
//        return n>=size ? Triangle() : tris[n+offset];
//    }
//    int getSize() const{
//        return size;
//    }
//
//    Mesh(sc::global_ptr<Triangle> &tri, MeshStage stg) : tris(tri){
//        material = stg.material;
//        attr_1 = stg.attr_1;
//        boundingVolume = stg.boundingVolume;
//        offset = stg.offset;
//        size = stg.size;
//    }
//    Mesh(Triangle *tri, MeshStage stg) : tris(tri){
//        material = stg.material;
//        attr_1 = stg.attr_1;
//        boundingVolume = stg.boundingVolume;
//        offset = stg.offset;
//        size = stg.size;
//    }
//
//
//    Mesh(sc::global_ptr<Triangle> &tri, sc::float3 color, int mat, int size, int offset) : tris(tri), attr_1(color), material(mat), size(size), offset(offset){
//        sc::float3 min, max;
//        for(int i = 0; i<size; i++){
//            Triangle t = getTriangle(i);
//            sc::float3 a, b, c;
//            a = t.v0;
//            b = t.v1;
//            c = t.v2;
//            //Now we set the maximums and minimums
//            {
//                max.x() = sc::max(max.x(), a.x());
//                max.x() = sc::max(max.x(), b.x());
//                max.x() = sc::max(max.x(), c.x());
//
//                max.y() = sc::max(max.y(), a.y());
//                max.y() = sc::max(max.y(), b.y());
//                max.y() = sc::max(max.y(), c.y());
//
//                max.z() = sc::max(max.z(), a.z());
//                max.z() = sc::max(max.z(), b.z());
//                max.z() = sc::max(max.z(), c.z());
//
//                min.x() = sc::min(min.x(), a.x());
//                min.x() = sc::min(min.x(), b.x());
//                min.x() = sc::min(min.x(), c.x());
//
//                min.y() = sc::min(min.y(), a.y());
//                min.y() = sc::min(min.y(), b.y());
//                min.y() = sc::min(min.y(), c.y());
//
//                min.z() = sc::min(min.z(), a.z());
//                min.z() = sc::min(min.z(), b.z());
//                min.z() = sc::min(min.z(), c.z());
//            }
//        }
//        // We now average maximums and minimums to find center, and substract min from max to get dimensions
//        boundingVolume = AABB(min, max);
//    }
//
//    Mesh(sc::global_ptr<Triangle> &tri, sc::float3 color, int mat, int size, int offset, AABB bVol) : tris(tri), attr_1(color), material(mat), size(size), offset(offset), boundingVolume(bVol){}
//    Mesh(Triangle *tri, sc::float3 color, int mat, int size, int offset, AABB bVol) : tris(tri), attr_1(color), material(mat), size(size), offset(offset), boundingVolume(bVol){}
//
//    material::intersectReturn tBoxIntersect(const Ray &r) const{
//        float minDist = FLT_MAX;
//        material::intersectReturn tobj = material::intersectReturn();
//        for (int k = 0; k < getSize(); k++) {
//            if(!getBoundingVolume().fRayIntersect(r))
//                break;
//            Triangle tr = getTriangle(k);
//            material::intersectReturn t = tr.rayIntersect(r);
//            Ray t_reflect = Ray(t.hitpoint, t.normal);
//            bool t_intersect = t.intersect;
//            t_intersect = t_intersect;
//
//            if (t_intersect) {
//                //float dist = sc::fast_length(r.getOrigin() - t_reflect.getOrigin());
//                if (minDist > t.intersectDistance) {
//                    minDist = t.intersectDistance;
//                    t.stackPos = k;
//                    t.objType=1;
//                    tobj = t;
//                }
//            }
//        }
//        return tobj;
//    }
//
//    std::vector<Triangle> getTriangleVec() const{
//        std::vector<Triangle> retVec = std::vector<Triangle>(tris+offset, tris+offset+size);
//        return retVec;
//    }
////    material::intersectReturn tBoxIntersect(const Ray &r) const{
////
////        material::intersectReturn ret;
////        if(!boundingVolume.fRayIntersect(r))
////            return ret;
////
////        float minDist = FLT_MAX;
////        Triangle temp;
////
////        for(int i = 0; i<size; i++){
////            Triangle tr = getTriangle(i);
////            material::intersectReturn t = tr.rayIntersect(r);
////            float dist = t.intersectDistance;
////            if(minDist > dist && t.intersect) {
////                temp = tr;
////                minDist = dist;
////                ret=t;
////            }
////        }
////
////        if(!ret.intersect)
////            return ret;
////
////        ret.hitpoint=r.getOrigin() + r.getDirection()*minDist;
////        ret.normal=temp.getNormal();
////        ret.intersectDistance=minDist;
////        ret.attr_1=temp.getAttr1();
////        ret.material=temp.getMaterial();
////        return ret;
////    }
//
//    const AABB &getBoundingVolume() const {
//        return boundingVolume;
//    }
////    material::intersectReturn rayIntersect(const Ray &r){
////
////    }
//};


#endif //PATHTRACER_SYCL_MESH_H