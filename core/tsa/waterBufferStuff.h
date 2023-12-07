//
// Created by tropi on 12/7/2023.
//

#pragma once
#include "../ew/external/glad.h"

namespace tsa {
    class WaterBuffers {
    public:
        WaterBuffers();
        void bindReflectionFrameBuffer();
        void bindRefractionFrameBuffer();
        void unbindCurrFrameBuffers();
        GLuint getReflectionText();
        void cleanUpTime();
    private:
        static const int REFLECTION_WIDTH = 320;
         static const int REFLECTION_HEIGHT = 180;

         static const int REFRACTION_WIDTH = 1280;
         static const int REFRACTION_HEIGHT = 720;

         GLuint reflectionFrameBuffer;
         GLuint reflectionTexture;
         GLuint reflectionDepthBuffer;

         GLuint refractionFrameBuffer;
         GLuint refractionTexture;
         GLuint refractionDepthTexture;

        void initReflectionFrameBuffer();
        void initRefractionFrameBuffer();
        int createFrameBuffer();
        int createTextureAttribute(int width, int height);
        int createDepthBufferAttribute(int width, int height);
        int createDepthTextureAttribute(int width, int height);
    };
}