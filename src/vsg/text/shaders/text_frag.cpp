#include <vsg/io/ReaderWriter_vsg.h>
static auto text_frag = []() {std::istringstream str(
"#vsga 0.0.2\n\
Root id=1 vsg::ShaderStage\n\
{\n\
  NumUserObjects 0\n\
  Stage 16\n\
  EntryPoint \"main\"\n\
  ShaderModule id=2 vsg::ShaderModule\n\
  {\n\
    NumUserObjects 0\n\
    Source \"#version 450\n\
\n\
layout(binding = 0) uniform sampler2D texSampler;\n\
\n\
layout(location = 0) in vec4 fragColor;\n\
layout(location = 1) in vec4 outlineColor;\n\
layout(location = 2) in float outlineWidth;\n\
layout(location = 3) in vec2 fragTexCoord;\n\
\n\
layout(location = 0) out vec4 outColor;\n\
\n\
vec2 glyph_alpha(vec2 texcoord, vec2 dx, vec2 dy)\n\
{\n\
    float lod = textureQueryLod(texSampler, texcoord).x;\n\
    float innerCutOff = 0.0;\n\
    if (lod>0.0) innerCutOff = lod * 0.03;\n\
\n\
    float scale = 1.0;\n\
    float distance_from_edge = (textureGrad(texSampler, texcoord, dx, dy).r);\n\
\n\
    float d_distance_dx = dFdx(distance_from_edge) * scale;\n\
    float d_distance_dy = dFdy(distance_from_edge) * scale;\n\
\n\
    float delta = sqrt(d_distance_dx * d_distance_dx + d_distance_dy * d_distance_dy);\n\
\n\
    float min_distance_from_edge = distance_from_edge - delta;\n\
    float max_distance_from_edge = distance_from_edge + delta;\n\
\n\
    //min_distance_from_edge += 0.0;\n\
    min_distance_from_edge += innerCutOff;\n\
    float inner_alpha = 0.0;\n\
    if (min_distance_from_edge >= 0.0) inner_alpha = 1.0;\n\
    else if (max_distance_from_edge >= 0.0) inner_alpha = max_distance_from_edge/(max_distance_from_edge-min_distance_from_edge);\n\
\n\
    min_distance_from_edge += outlineWidth;\n\
    float outer_alpha = 0.0;\n\
    if (min_distance_from_edge >= 0.0) outer_alpha = 1.0;\n\
    else if (max_distance_from_edge >= 0.0) outer_alpha = max_distance_from_edge/(max_distance_from_edge-min_distance_from_edge);\n\
\n\
    return vec2(inner_alpha, outer_alpha);\n\
}\n\
\n\
vec2 sampled_glyph_alpha_grid(vec2 texcoord)\n\
{\n\
    float lod = textureQueryLod(texSampler, texcoord).x;\n\
    vec2 dx = dFdx(texcoord);\n\
    vec2 dy = dFdy(texcoord);\n\
\n\
    if (lod<=0.0) return glyph_alpha(texcoord, dx, dy);\n\
\n\
    float area = length(dx) * length(dy);\n\
    float average_side = sqrt(area) / (1.0 + lod);\n\
    float num_x = ceil(length(dx) / average_side);\n\
    float num_y = ceil(length(dy) / average_side);\n\
\n\
    vec2 interval_dx = dx / num_x;\n\
    vec2 interval_dy = dy / num_y;\n\
\n\
    vec2 total_alpha = vec2(0.0, 0.0);\n\
    vec2 tc_row_start = texcoord - dx*0.5 - dy*0.5;\n\
    for(float r = 0; r<num_y; ++r)\n\
    {\n\
        vec2 tc = tc_row_start;\n\
        tc_row_start = tc_row_start + interval_dy;\n\
\n\
        for(float c = 0; c<num_x; ++c)\n\
        {\n\
            total_alpha = total_alpha + glyph_alpha(tc, interval_dx, interval_dy);\n\
\n\
            tc = tc + interval_dx;\n\
        }\n\
    }\n\
\n\
    return total_alpha / (num_x * num_y);\n\
}\n\
\n\
void main()\n\
{\n\
    vec2 alphas = sampled_glyph_alpha_grid(fragTexCoord);\n\
\n\
    if (alphas[1]>0.0)\n\
    {\n\
        vec4 glyph = vec4(fragColor.rgb, fragColor.a * alphas[0]);\n\
        vec4 outline = vec4(outlineColor.rgb, outlineColor.a * alphas[1]);\n\
        outColor = mix(outline, glyph, glyph.a);\n\
    }\n\
    else\n\
    {\n\
        outColor = vec4(fragColor.rgb,  fragColor.a * alphas[0]);\n\
    }\n\
\n\
    if (outColor.a == 0.0) discard;\n\
}\n\
\"\n\
    SPIRVSize 1807\n\
    SPIRV 119734787 65536 524298 305 0 131089 1 131089 50 393227 1 1280527431\n\
     1685353262 808793134 0 196622 0 1 655375 4 4 1852399981 0 97\n\
     238 251 266 279 196624 4 7 196611 2 450 262149 4\n\
     1852399981 0 589829 13 1887005799 1818320744 677472368 993158774 993158774 993158774 0 327685\n\
     10 1668834676 1685221231 0 196613 11 30820 196613 12 31076 655365 17\n\
     1886216563 1600415084 1887005799 1818320744 1600219248 1684632167 845575720 59 327685 16 1668834676 1685221231\n\
     0 196613 20 6582124 327685 24 1400399220 1819307361 29285 327685 31 1701736041\n\
     1953842034 6710863 262149 41 1818321779 101 458757 43 1953720676 1701015137 1869768287 1684365165\n\
     25959 393221 51 1768185700 1851880563 1683973475 120 393221 56 1768185700 1851880563 1683973475\n\
     121 262149 61 1953260900 97 524293 70 1601071469 1953720676 1701015137 1869768287 1684365165\n\
     25959 524293 74 1601724781 1953720676 1701015137 1869768287 1684365165 25959 327685 81 1701736041\n\
     1818320754 6383728 393221 97 1819571567 1466265193 1752458345 0 327685 101 1702131055 1818320754\n\
     6383728 196613 121 6582124 196613 126 30820 196613 129 31076 262149 136\n\
     1634886000 109 262149 138 1634886000 109 262149 140 1634886000 109 262149 144\n\
     1634038369 0 393221 150 1919252065 1600481121 1701079411 0 262149 156 1601009006 120\n\
     262149 162 1601009006 121 327685 168 1702129257 1818326642 7890015 327685 173 1702129257\n\
     1818326642 7955551 327685 178 1635020660 1818320748 6383728 393221 180 1918854004 1935636335 1953653108\n\
     0 196613 189 114 196613 198 25460 196613 203 99 262149 213\n\
     1634886000 109 262149 215 1634886000 109 262149 217 1634886000 109 262149 236\n\
     1752198241 29537 393221 238 1734439526 1131963732 1685221231 0 262149 239 1634886000 109\n\
     262149 249 1887005799 104 327685 251 1734439526 1869377347 114 262149 265 1819571567\n\
     6647401 393221 266 1819571567 1130720873 1919904879 0 327685 279 1131705711 1919904879 0\n\
     262215 24 34 0 262215 24 33 0 262215 97 30 2\n\
     262215 238 30 3 262215 251 30 0 262215 266 30 1\n\
     262215 279 30 0 131091 2 196641 3 2 196630 6 32\n\
     262167 7 6 2 262176 8 7 7 393249 9 7 8\n\
     8 8 262177 15 7 8 262176 19 7 6 589849 21\n\
     6 1 0 0 0 1 0 196635 22 21 262176 23\n\
     0 22 262203 23 24 0 262165 28 32 0 262187 28\n\
     29 0 262187 6 32 0 131092 34 262187 6 39 1022739087\n\
     262187 6 42 1065353216 262167 48 6 4 262176 96 1 6\n\
     262203 96 97 1 327724 7 179 32 32 262187 6 183\n\
     1056964608 262176 237 1 7 262203 237 238 1 262187 28 242\n\
     1 262176 248 7 48 262176 250 1 48 262203 250 251\n\
     1 262167 252 6 3 262187 28 255 3 262203 250 266\n\
     1 262176 278 3 48 262203 278 279 3 262176 298 3\n\
     6 327734 2 4 0 3 131320 5 262203 8 236 7\n\
     262203 8 239 7 262203 248 249 7 262203 248 265 7\n\
     262205 7 240 238 196670 239 240 327737 7 241 17 239\n\
     196670 236 241 327745 19 243 236 242 262205 6 244 243\n\
     327866 34 245 244 32 196855 247 0 262394 245 246 286\n\
     131320 246 262205 48 253 251 524367 252 254 253 253 0\n\
     1 2 327745 96 256 251 255 262205 6 257 256 327745\n\
     19 258 236 29 262205 6 259 258 327813 6 260 257\n\
     259 327761 6 261 254 0 327761 6 262 254 1 327761\n\
     6 263 254 2 458832 48 264 261 262 263 260 196670\n\
     249 264 262205 48 267 266 524367 252 268 267 267 0\n\
     1 2 327745 96 269 266 255 262205 6 270 269 327745\n\
     19 271 236 242 262205 6 272 271 327813 6 273 270\n\
     272 327761 6 274 268 0 327761 6 275 268 1 327761\n\
     6 276 268 2 458832 48 277 274 275 276 273 196670\n\
     265 277 262205 48 280 265 262205 48 281 249 327745 19\n\
     282 249 255 262205 6 283 282 458832 48 284 283 283\n\
     283 283 524300 48 285 1 46 280 281 284 196670 279\n\
     285 131321 247 131320 286 262205 48 287 251 524367 252 288\n\
     287 287 0 1 2 327745 96 289 251 255 262205 6\n\
     290 289 327745 19 291 236 29 262205 6 292 291 327813\n\
     6 293 290 292 327761 6 294 288 0 327761 6 295\n\
     288 1 327761 6 296 288 2 458832 48 297 294 295\n\
     296 293 196670 279 297 131321 247 131320 247 327745 298 299\n\
     279 255 262205 6 300 299 327860 34 301 300 32 196855\n\
     303 0 262394 301 302 303 131320 302 65788 131320 303 65789\n\
     65592 327734 7 13 0 9 196663 8 10 196663 8 11\n\
     196663 8 12 131320 14 262203 19 20 7 262203 19 31\n\
     7 262203 19 41 7 262203 19 43 7 262203 19 51\n\
     7 262203 19 56 7 262203 19 61 7 262203 19 70\n\
     7 262203 19 74 7 262203 19 81 7 262203 19 101\n\
     7 262205 22 25 24 262205 7 26 10 327785 7 27\n\
     25 26 327761 6 30 27 0 196670 20 30 196670 31\n\
     32 262205 6 33 20 327866 34 35 33 32 196855 37\n\
     0 262394 35 36 37 131320 36 262205 6 38 20 327813\n\
     6 40 38 39 196670 31 40 131321 37 131320 37 196670\n\
     41 42 262205 22 44 24 262205 7 45 10 262205 7\n\
     46 11 262205 7 47 12 524376 48 49 44 45 4\n\
     46 47 327761 6 50 49 0 196670 43 50 262205 6\n\
     52 43 262351 6 53 52 262205 6 54 41 327813 6\n\
     55 53 54 196670 51 55 262205 6 57 43 262352 6\n\
     58 57 262205 6 59 41 327813 6 60 58 59 196670\n\
     56 60 262205 6 62 51 262205 6 63 51 327813 6\n\
     64 62 63 262205 6 65 56 262205 6 66 56 327813\n\
     6 67 65 66 327809 6 68 64 67 393228 6 69\n\
     1 31 68 196670 61 69 262205 6 71 43 262205 6\n\
     72 61 327811 6 73 71 72 196670 70 73 262205 6\n\
     75 43 262205 6 76 61 327809 6 77 75 76 196670\n\
     74 77 262205 6 78 31 262205 6 79 70 327809 6\n\
     80 79 78 196670 70 80 196670 81 32 262205 6 82\n\
     70 327870 34 83 82 32 196855 85 0 262394 83 84\n\
     86 131320 84 196670 81 42 131321 85 131320 86 262205 6\n\
     87 74 327870 34 88 87 32 196855 90 0 262394 88\n\
     89 90 131320 89 262205 6 91 74 262205 6 92 74\n\
     262205 6 93 70 327811 6 94 92 93 327816 6 95\n\
     91 94 196670 81 95 131321 90 131320 90 131321 85 131320\n\
     85 262205 6 98 97 262205 6 99 70 327809 6 100\n\
     99 98 196670 70 100 196670 101 32 262205 6 102 70\n\
     327870 34 103 102 32 196855 105 0 262394 103 104 106\n\
     131320 104 196670 101 42 131321 105 131320 106 262205 6 107\n\
     74 327870 34 108 107 32 196855 110 0 262394 108 109\n\
     110 131320 109 262205 6 111 74 262205 6 112 74 262205\n\
     6 113 70 327811 6 114 112 113 327816 6 115 111\n\
     114 196670 101 115 131321 110 131320 110 131321 105 131320 105\n\
     262205 6 116 81 262205 6 117 101 327760 7 118 116\n\
     117 131326 118 65592 327734 7 17 0 15 196663 8 16\n\
     131320 18 262203 19 121 7 262203 8 126 7 262203 8\n\
     129 7 262203 8 136 7 262203 8 138 7 262203 8\n\
     140 7 262203 19 144 7 262203 19 150 7 262203 19\n\
     156 7 262203 19 162 7 262203 8 168 7 262203 8\n\
     173 7 262203 8 178 7 262203 8 180 7 262203 19\n\
     189 7 262203 8 198 7 262203 19 203 7 262203 8\n\
     213 7 262203 8 215 7 262203 8 217 7 262205 22\n\
     122 24 262205 7 123 16 327785 7 124 122 123 327761\n\
     6 125 124 0 196670 121 125 262205 7 127 16 262351\n\
     7 128 127 196670 126 128 262205 7 130 16 262352 7\n\
     131 130 196670 129 131 262205 6 132 121 327868 34 133\n\
     132 32 196855 135 0 262394 133 134 135 131320 134 262205\n\
     7 137 16 196670 136 137 262205 7 139 126 196670 138\n\
     139 262205 7 141 129 196670 140 141 458809 7 142 13\n\
     136 138 140 131326 142 131320 135 262205 7 145 126 393228\n\
     6 146 1 66 145 262205 7 147 129 393228 6 148\n\
     1 66 147 327813 6 149 146 148 196670 144 149 262205\n\
     6 151 144 393228 6 152 1 31 151 262205 6 153\n\
     121 327809 6 154 42 153 327816 6 155 152 154 196670\n\
     150 155 262205 7 157 126 393228 6 158 1 66 157\n\
     262205 6 159 150 327816 6 160 158 159 393228 6 161\n\
     1 9 160 196670 156 161 262205 7 163 129 393228 6\n\
     164 1 66 163 262205 6 165 150 327816 6 166 164\n\
     165 393228 6 167 1 9 166 196670 162 167 262205 7\n\
     169 126 262205 6 170 156 327760 7 171 170 170 327816\n\
     7 172 169 171 196670 168 172 262205 7 174 129 262205\n\
     6 175 162 327760 7 176 175 175 327816 7 177 174\n\
     176 196670 173 177 196670 178 179 262205 7 181 16 262205\n\
     7 182 126 327822 7 184 182 183 327811 7 185 181\n\
     184 262205 7 186 129 327822 7 187 186 183 327811 7\n\
     188 185 187 196670 180 188 196670 189 32 131321 190 131320\n\
     190 262390 192 193 0 131321 194 131320 194 262205 6 195\n\
     189 262205 6 196 162 327864 34 197 195 196 262394 197\n\
     191 192 131320 191 262205 7 199 180 196670 198 199 262205\n\
     7 200 180 262205 7 201 173 327809 7 202 200 201\n\
     196670 180 202 196670 203 32 131321 204 131320 204 262390 206\n\
     207 0 131321 208 131320 208 262205 6 209 203 262205 6\n\
     210 156 327864 34 211 209 210 262394 211 205 206 131320\n\
     205 262205 7 212 178 262205 7 214 198 196670 213 214\n\
     262205 7 216 168 196670 215 216 262205 7 218 173 196670\n\
     217 218 458809 7 219 13 213 215 217 327809 7 220\n\
     212 219 196670 178 220 262205 7 221 198 262205 7 222\n\
     168 327809 7 223 221 222 196670 198 223 131321 207 131320\n\
     207 262205 6 224 203 327809 6 225 224 42 196670 203\n\
     225 131321 204 131320 206 131321 193 131320 193 262205 6 226\n\
     189 327809 6 227 226 42 196670 189 227 131321 190 131320\n\
     192 262205 7 228 178 262205 6 229 156 262205 6 230\n\
     162 327813 6 231 229 230 327760 7 232 231 231 327816\n\
     7 233 228 232 131326 233 65592\n\
  }\n\
  NumSpecializationConstants 0\n\
}\n\
");
vsg::ReaderWriter_vsg io;
return io.read_cast<vsg::ShaderStage>(str);
};