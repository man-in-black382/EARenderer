/*
The MIT License (MIT)

Copyright (c) 2012-2016 Syoyo Fujita and many contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

//
// version 1.0.0 : Change data structure. Change license from BSD to MIT.
//

//
// Use this in *one* .cc
//   #define TINYOBJLOADER_IMPLEMENTATION
//   #include "tiny_obj_loader.h"
//

#ifndef TINY_OBJ_LOADER_H_
#define TINY_OBJ_LOADER_H_

#include <map>
#include <string>
#include <vector>

namespace tinyobj {

    typedef struct {
        std::string name;

        float ambient[3];
        float diffuse[3];
        float specular[3];
        float transmittance[3];
        float emission[3];
        float shininess;
        float ior;       // index of refraction
        float dissolve;  // 1 == opaque; 0 == fully transparent
        // illumination model (see http://www.fileformat.info/format/material/)
        int illum;

        int dummy;  // Suppress padding warning.

        std::string ambient_texname;             // map_Ka
        std::string diffuse_texname;             // map_Kd
        std::string specular_texname;            // map_Ks
        std::string specular_highlight_texname;  // map_Ns
        std::string bump_texname;                // map_bump, bump
        std::string displacement_texname;        // disp
        std::string alpha_texname;               // map_d

        // PBR extension
        // http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr
        float roughness;                // [0, 1] default 0
        float metallic;                 // [0, 1] default 0
        float sheen;                    // [0, 1] default 0
        float clearcoat_thickness;      // [0, 1] default 0
        float clearcoat_roughness;      // [0, 1] default 0
        float anisotropy;               // aniso. [0, 1] default 0
        float anisotropy_rotation;      // anisor. [0, 1] default 0
        std::string roughness_texname;  // map_Pr
        std::string metallic_texname;   // map_Pm
        std::string sheen_texname;      // map_Ps
        std::string emissive_texname;   // map_Ke
        std::string normal_texname;     // norm. For normal mapping.

        std::map<std::string, std::string> unknown_parameter;
    } material_t;

    typedef struct {
        std::string name;

        std::vector<int> intValues;
        std::vector<float> floatValues;
        std::vector<std::string> stringValues;
    } tag_t;

// Index struct to support differnt indices for vtx/normal/texcoord.
// -1 means not used.
    typedef struct {
        int vertex_index;
        int normal_index;
        int texcoord_index;
    } index_t;

    typedef struct {
        std::vector<index_t> indices;
        std::vector<unsigned char> num_face_vertices;  // The number of vertices per
        // face. 3 = polygon, 4 = quad,
        // ... Up to 255.
        std::vector<int> material_ids;                 // per-face material ID
        std::vector<tag_t> tags;                       // SubD tag
    } mesh_t;

    typedef struct {
        std::string name;
        mesh_t mesh;
    } shape_t;

// Vertex attributes
    typedef struct {
        std::vector<float> vertices;   // 'v'
        std::vector<float> normals;    // 'vn'
        std::vector<float> texcoords;  // 'vt'
    } attrib_t;

    typedef struct callback_t_ {
        // W is optional and set to 1 if there is no `w` item in `v` line
        void (*vertex_cb)(void *user_data, float x, float y, float z, float w);

        void (*normal_cb)(void *user_data, float x, float y, float z);

        // y and z are optional and set to 0 if there is no `y` and/or `z` item(s) in
        // `vt` line.
        void (*texcoord_cb)(void *user_data, float x, float y, float z);

        // called per 'f' line. num_indices is the number of face indices(e.g. 3 for
        // triangle, 4 for quad)
        // 0 will be passed for undefined index in index_t members.
        void (*index_cb)(void *user_data, index_t *indices, int num_indices);

        // `name` material name, `material_id` = the array index of material_t[]. -1
        // if
        // a material not found in .mtl
        void (*usemtl_cb)(void *user_data, const char *name, int material_id);

        // `mCookTorranceMaterials` = parsed material data.
        void (*mtllib_cb)(void *user_data, const material_t *materials,
                int num_materials);

        // There may be multiple group names
        void (*group_cb)(void *user_data, const char **names, int num_names);

        void (*object_cb)(void *user_data, const char *name);

        callback_t_()
                : vertex_cb(NULL),
                  normal_cb(NULL),
                  texcoord_cb(NULL),
                  index_cb(NULL),
                  usemtl_cb(NULL),
                  mtllib_cb(NULL),
                  group_cb(NULL),
                  object_cb(NULL) {
        }
    } callback_t;

    class MaterialReader {
    public:
        MaterialReader() {
        }

        virtual ~MaterialReader();

        virtual bool operator()(const std::string &matId,
                std::vector<material_t> *materials,
                std::map<std::string, int> *matMap,
                std::string *err) = 0;
    };

    class MaterialFileReader : public MaterialReader {
    public:
        explicit MaterialFileReader(const std::string &mtl_basepath)
                : m_mtlBasePath(mtl_basepath) {
        }

        virtual ~MaterialFileReader() {
        }

        virtual bool operator()(const std::string &matId,
                std::vector<material_t> *materials,
                std::map<std::string, int> *matMap, std::string *err);

    private:
        std::string m_mtlBasePath;
    };

/// Loads .obj from a file.
/// 'attrib', 'shapes' and 'materials' will be filled with parsed shape data
/// 'shapes' will be filled with parsed shape data
/// Returns true when loading .obj become success.
/// Returns warning and error message into `err`
/// 'mtl_basepath' is optional, and used for base path for .mtl file.
/// 'triangulate' is optional, and used whether triangulate polygon face in .obj
/// or not.
    bool LoadObj(attrib_t *attrib, std::vector<shape_t> *shapes,
            std::vector<material_t> *materials, std::string *err,
            const char *filename, const char *mtl_basepath = NULL,
            bool triangulate = true);

/// Loads .obj from a file with custom user callback.
/// .mtl is loaded as usual and parsed material_t data will be passed to
/// `callback.mtllib_cb`.
/// Returns true when loading .obj/.mtl become success.
/// Returns warning and error message into `err`
/// See `examples/callback_api/` for how to use this function.
    bool LoadObjWithCallback(std::istream &inStream, const callback_t &callback,
            void *user_data = NULL,
            MaterialReader *readMatFn = NULL,
            std::string *err = NULL);

/// Loads object from a std::istream, uses GetMtlIStreamFn to retrieve
/// std::istream for materials.
/// Returns true when loading .obj become success.
/// Returns warning and error message into `err`
    bool LoadObj(attrib_t *attrib, std::vector<shape_t> *shapes,
            std::vector<material_t> *materials, std::string *err,
            std::istream *inStream, MaterialReader *readMatFn,
            bool triangulate = true);

/// Loads materials into std::map
    void LoadMtl(std::map<std::string, int> *material_map,
            std::vector<material_t> *materials, std::istream *inStream);

}  // namespace tinyobj

#endif
