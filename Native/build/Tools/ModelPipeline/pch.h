#pragma once

#include "targetver.h"

#include <stdio.h>

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <wrl.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>

#include "Utility.h"
#include "Model.h"
#include "Mesh.h"
#include "ModelMaterial.h"

#include "ModelProcessor.h"
#include "MeshProcessor.h"
#include "ModelMaterialProcessor.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>