#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Shader.h"

class ShaderManager {
public:
    std::unordered_map<std::string, Shader*> shaders;
    
    static ShaderManager& Instance() {
        static ShaderManager instance;
        return instance;
    }
    
    Shader* LoadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
        if (shaders.find(name) != shaders.end()) {
            return shaders[name];
        }
        
        std::string vertexCode = LoadShaderFile(vertexPath);
        std::string fragmentCode = LoadShaderFile(fragmentPath);
        
        if (vertexCode.empty() || fragmentCode.empty()) {
            std::cout << "Failed to load shader files for: " << name << std::endl;
            return nullptr;
        }
        
        Shader* shader = new Shader(vertexCode, fragmentCode, true);
        shaders[name] = shader;
        
        std::cout << "Loaded shader: " << name << std::endl;
        return shader;
    }
    
    Shader* GetShader(const std::string& name) {
        auto it = shaders.find(name);
        if (it != shaders.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    void ReloadShader(const std::string& name) {
        auto it = shaders.find(name);
        if (it != shaders.end()) {
            delete it->second;
            shaders.erase(it);
        }
    }
    
    void ReloadAllShaders() {
        for (auto& pair : shaders) {
            delete pair.second;
        }
        shaders.clear();
        std::cout << "All shaders cleared. Reload manually." << std::endl;
    }
    
    ~ShaderManager() {
        for (auto& pair : shaders) {
            delete pair.second;
        }
    }

private:
    std::string LoadShaderFile(const std::string& path) {
        std::ifstream file;
        std::stringstream stream;
        
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        
        try {
            file.open(path);
            stream << file.rdbuf();
            file.close();
            return stream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << path << std::endl;
            std::cout << "Error: " << e.what() << std::endl;
            return "";
        }
    }
};