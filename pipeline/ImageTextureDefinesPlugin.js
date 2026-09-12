const path = require("path");
const fs = require("fs");
const Util = require("../lib/framework64/pipeline/Util");

class ImageTextureDefinesPlugin {
    _environment

    initialize(environment) {
        this._environment = environment;
    }

    postProcessMesh(meshJson, gltfLoader) {
        this._writeImageTextureDefines(meshJson, gltfLoader);
    }

    postProcessSkinnedMesh(meshJson, gltfLoader) {
        this._writeImageTextureDefines(meshJson, gltfLoader);
    }

    _writeImageTextureDefines(meshJson, gltfLoader) {
        if (gltfLoader.images.length === 0)
            return;

        const meshName = !!meshJson.name ? meshJson.name : path.basename(meshJson.src, path.extname(meshJson.src));
        const fileName = Util.safeDefineName(meshName) + "_image_texture_defs.h";
        const destPath = path.join(this._environment.includeDirectory, fileName);
        this._writeToFile(gltfLoader, meshName, destPath);
    }

    _writeToFile(gltfLoader, name, destFile) {
        const imageToTextureMap = this._buildImageToTextureMap(gltfLoader);
    
        const file = fs.openSync(destFile, "w");
    
        console.log(`ImageTextureDefinesPlugin: Writing ${destFile}`);
        fs.writeSync(file, "#pragma once\n\n");
    
        imageToTextureMap.forEach((textureArray, imageIndex) => {
            const image = gltfLoader.images[imageIndex];
            const imageName = Util.safeDefineName(Object.hasOwnProperty(image, "name") ? image.name : path.basename(image.src, path.extname(image.src)));
    
            for (let i = 0; i < textureArray.length; i++) {
                fs.writeSync(file, `#define ${name}_image_${imageName}_texture_${i} ${textureArray[i]}\n`);
            }
        });
    
        fs.writeSync(file, '\n');
        fs.closeSync(file);
    }

    _buildImageToTextureMap(gltfLoader) {
        const imageToTextureMap = new Map();
    
        for(let i = 0; i < gltfLoader.textures.length; i++) {
            const texture = gltfLoader.textures[i];
    
            if (imageToTextureMap.has(texture.image)) {
                const textureArray = imageToTextureMap.get(texture.image);
                textureArray.push(i);
            }
            else {
                imageToTextureMap.set(texture.image, [i]);
            }
        }
    
        return imageToTextureMap;
    }
}

module.exports = ImageTextureDefinesPlugin;