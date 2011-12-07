/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

static int VERTEX_COORDS_ATTR = 0;
static int TEXTURE_COORDS_ATTR = 1;

static int PROJECTION_MATRIX_UNIFORM = 0;
static int TEXTURE_TRANSFORM_MATRIX_UNIFORM = 1;
static int COLOR_UNIFORM = 2;
static int OPACITY_UNIFORM = 3;
static int TEXTURE_UNIFORM = 4;

static const std::string glslMainVertexShader = "\n\
    highp vec4 calculatePosition();\n\
    void main() {\n\
        gl_Position = calculatePosition();\n\
    }\n";

static const std::string glslMainWithTexCoordsVertexShader = "\n\
    attribute vec2 textureCoord;\n\
    uniform mat2 textureTransformMatrix;\n\
    varying vec2 textureCoords;\n\
    vec4 calculatePosition();\n\
    void main()\n\
    {\n\
        gl_Position = calculatePosition();\n\
        textureCoords = textureTransformMatrix * textureCoord;\n\
    }\n";

static std::string glslPositionOnlyVertexShader = "\n\
    attribute vec2 vertexCoord;\n\
    uniform mat3 projectionMatrix;\n\
    vec4 calculatePosition() {\n\
        return vec4(projectionMatrix * vec3(vertexCoord.xy, 1), 1);\n\
    }\n";

static const std::string glslMainFragmentShader = "\n\
    uniform lowp float opacity;\n\
    lowp vec4 calculatePixel();\n\
    void main()\n\
    {\n\
        gl_FragColor = calculatePixel() * opacity;\n\
    }\n";

static const std::string glslTextureSrcFragmentShader = "\n\
    varying mediump vec2 textureCoords;\n\
    uniform lowp vec4 color;\n\
    uniform sampler2D texture;\n\
    vec4 calculatePixel() {\n\
        return texture2D(texture, textureCoords) * color;\n\
    }\n";

static const std::string glslSolidColorFragmentShader = "\n\
    uniform lowp vec4 color;\n\
    vec4 calculatePixel() {\n\
        return color;\n\
    }\n";
