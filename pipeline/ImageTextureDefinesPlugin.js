const path = require("path");
const fs = require("fs");
const Util = require("../lib/framework64/pipeline/Util");

class ImageTextureDefinesPlugin
{
    assetBundle;
    baseDirectory;
    outputDirectory;
    assetIncludeDirectory;
    platform;

    initialize(assetBundle, baseDirectory, outputDirectory, assetIncludeDirectory, platform) {
        this.assetBundle = assetBundle;
        this.baseDirectory = baseDirectory;
        this.outputDirectory = outputDirectory;
        this.platform = platform;
        this.assetIncludeDirectory = assetIncludeDirectory;
    }

    meshParsed(meshJson, gltfLoader) {
        this._writeImageTextureDefines(meshJson, gltfLoader);
    }

    skinnedMeshParsed(meshJson, gltfLoader) {
        this._writeImageTextureDefines(meshJson, gltfLoader);
    }

    _writeImageTextureDefines(meshJson, gltfLoader) {
        if (gltfLoader.resources.images.length === 0)
            return;

        const meshName = !!meshJson.name ? meshJson.name : path.basename(meshJson.src, path.extname(meshJson.src));
        const fileName = Util.safeDefineName(meshName) + "_image_texture_defs.h";
        const destPath = path.join(this.assetIncludeDirectory, fileName);
        this._writeToFile(gltfLoader, meshName, destPath);
    }

    _writeToFile(gltfLoader, name, destFile) {
        const imageToTextureMap = this._buildImageToTextureMap(gltfLoader);
    
        const file = fs.openSync(destFile, "w");
    
        fs.writeSync(file, "#pragma once\n\n");
    
        imageToTextureMap.forEach((textureArray, imageIndex) => {
            const imageName = Util.safeDefineName(gltfLoader.resources.images[imageIndex].name);
    
            for (let i = 0; i < textureArray.length; i++) {
                fs.writeSync(file, `#define ${name}_image_${imageName}_texture_${i} ${textureArray[i]}\n`);
            }
        });
    
        fs.writeSync(file, '\n');
        fs.closeSync(file);
    }

    _buildImageToTextureMap(gltfLoader) {
        const imageToTextureMap = new Map();
    
        for(let i = 0; i < gltfLoader.resources.textures.length; i++) {
            const texture = gltfLoader.resources.textures[i];
    
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