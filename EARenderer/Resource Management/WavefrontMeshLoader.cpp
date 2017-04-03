//
//  WavefrontMeshLoader.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "WavefrontMeshLoader.hpp"

#define TINYOBJ_LOADER_OPT_IMPLEMENTATION // define this in only *one* .cc
#include "tinyobj_loader_opt.h"

namespace EARenderer {
    
    const char *mmap_file(size_t *len, const char* filename)
    {
        (*len) = 0;
#ifdef _WIN64
        HANDLE file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        assert(file != INVALID_HANDLE_VALUE);
        
        HANDLE fileMapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
        assert(fileMapping != INVALID_HANDLE_VALUE);
        
        LPVOID fileMapView = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
        auto fileMapViewChar = (const char*)fileMapView;
        assert(fileMapView != NULL);
#else
        
        FILE* f = fopen(filename, "r");
        fseek(f, 0, SEEK_END);
        long fileSize = ftell(f);
        fclose(f);
        
        struct stat sb;
        char *p;
        int fd;
        
        fd = open (filename, O_RDONLY);
        if (fd == -1) {
            perror ("open");
            return NULL;
        }
        
        if (fstat (fd, &sb) == -1) {
            perror ("fstat");
            return NULL;
        }
        
        if (!S_ISREG (sb.st_mode)) {
            fprintf (stderr, "%s is not a file\n", "lineitem.tbl");
            return NULL;
        }
        
        p = (char*)mmap (0, fileSize, PROT_READ, MAP_SHARED, fd, 0);
        
        if (p == MAP_FAILED) {
            perror ("mmap");
            return NULL;
        }
        
        if (close (fd) == -1) {
            perror ("close");
            return NULL;
        }
        
        (*len) = fileSize;
        
        return p;
        
#endif
    }
    
    WavefrontMeshLoader::WavefrontMeshLoader(const std::string& meshPath)
    :
    mMeshPath(meshPath)
    { }
    
    std::vector<SubMesh> WavefrontMeshLoader::load() {
        
        std::vector<SubMesh> subMeshes;
        
        tinyobj_opt::attrib_t attrib;
        std::vector<tinyobj_opt::shape_t> shapes;
        std::vector<tinyobj_opt::material_t> materials;
        
        size_t data_len = 0;
        const char* data = mmap_file(&data_len, mMeshPath.c_str());
        if (!data) {
            std::cerr << "Failed to read .obj file" << std::endl;
        }
        
        tinyobj_opt::LoadOption option;
        option.req_num_threads = 8;
        option.verbose = true;
        bool ret = parseObj(&attrib, &shapes, &materials, data, data_len, option);
        
        if (!ret) {
            std::cerr << "Failed to parse .obj" << std::endl;
        } 
        
        // bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
        // bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();
        
        size_t indicesPerFaceLine = 3;
        for (size_t s = 0; s < shapes.size(); s++) {
            std::vector<Vertex1P1N1UV> vertices;

            size_t toValue = (shapes[s].face_offset + shapes[s].length) * indicesPerFaceLine;
            for (size_t i = shapes[s].face_offset * indicesPerFaceLine; i < toValue; i++) {
                tinyobj_opt::index_t idx = attrib.indices[s + i];
                
                glm::vec4 position;
                position.w = 1.0;
                
                for (int k = 0; k < 3; k++) {
                    int vi = idx.vertex_index;
                    assert(vi >= 0);
                    
                    position[k] = attrib.vertices[3 * vi + k];
                    
                    //    bmin[k] = std::min(v[0][k], bmin[k]);
                    //  bmin[k] = std::min(v[1][k], bmin[k]);
                    //  bmin[k] = std::min(v[2][k], bmin[k]);
                    //  bmax[k] = std::max(v[0][k], bmax[k]);
                    //  bmax[k] = std::max(v[1][k], bmax[k]);
                    // bmax[k] = std::max(v[2][k], bmax[k]);
                }
                                
                glm::vec3 normal;
                
                if (attrib.normals.size() > 0) {
                    int nf = idx.normal_index;
                    if (nf >= 0) {
                        assert(3 * nf + 2 < attrib.normals.size());
                        for (int k = 0; k < 3; k++) {
                            normal[k] = attrib.normals[3 * nf + k];
                        }
                    }
                }
                
                glm::vec2 texcoords;
                
                if (attrib.texcoords.size() > 0) {
                    int tf = idx.texcoord_index;
                    assert(tf >= 0);
                    
                    for (int k = 0; k < 2; k++) {
                        texcoords[k] = attrib.texcoords[2 * tf + k];
                    }
                }
                
                vertices.emplace_back(position, texcoords, normal);
            }
            
            subMeshes.emplace_back(vertices);
        }
        return subMeshes;
    }
}
