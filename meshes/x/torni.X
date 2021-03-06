xof 0303txt 0032
template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template Frame {
 <3d82ab46-62da-11cf-ab39-0020af71e433>
 [...]
}

template Matrix4x4 {
 <f6f23f45-7686-11cf-8f52-0040333594a3>
 array FLOAT matrix[16];
}

template FrameTransformMatrix {
 <f6f23f41-7686-11cf-8f52-0040333594a3>
 Matrix4x4 frameMatrix;
}

template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshNormals {
 <f6f23f43-7686-11cf-8f52-0040333594a3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template Coords2d {
 <f6f23f44-7686-11cf-8f52-0040333594a3>
 FLOAT u;
 FLOAT v;
}

template MeshTextureCoords {
 <f6f23f40-7686-11cf-8f52-0040333594a3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}


Material PDX01_-_Default {
 0.803922;0.713726;0.596078;1.000000;;
 3.200000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;
}

Frame Box01 {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh  {
  52;
  -30.000000;0.000000;-30.000000;,
  30.000000;0.000000;-30.000000;,
  -30.000000;0.000000;30.000000;,
  30.000000;0.000000;30.000000;,
  -20.000000;40.000000;-20.000000;,
  20.000000;40.000000;-20.000000;,
  -20.000000;40.000000;20.000000;,
  20.000000;40.000000;20.000000;,
  20.000000;40.000000;10.000000;,
  -20.000000;40.000000;10.000000;,
  20.000000;40.000000;-9.999998;,
  -20.000000;40.000000;-10.000001;,
  -10.000000;40.000000;-20.000000;,
  -10.000000;40.000000;20.000000;,
  -10.000000;40.000000;10.000000;,
  -10.000000;40.000000;-10.000000;,
  10.000001;40.000000;20.000000;,
  9.999998;40.000000;10.000000;,
  9.999998;40.000000;-9.999998;,
  9.999998;40.000000;-20.000000;,
  -20.000000;50.000000;-20.000000;,
  -10.000000;50.000000;-20.000000;,
  -10.000000;50.000000;-10.000000;,
  -20.000000;50.000000;-10.000001;,
  20.000000;50.000000;20.000000;,
  10.000001;50.000000;20.000000;,
  9.999998;50.000000;10.000000;,
  20.000000;50.000000;10.000000;,
  -20.000000;50.000000;20.000000;,
  -20.000000;50.000000;10.000000;,
  -10.000000;50.000000;10.000000;,
  -10.000000;50.000000;20.000000;,
  20.000000;50.000000;-20.000000;,
  20.000000;50.000000;-9.999998;,
  9.999998;50.000000;-9.999998;,
  9.999998;50.000000;-20.000000;,
  30.000000;0.000000;-30.000000;,
  20.000000;40.000000;-20.000000;,
  30.000000;0.000000;30.000000;,
  20.000000;40.000000;20.000000;,
  -30.000000;0.000000;30.000000;,
  -20.000000;40.000000;-20.000000;,
  -30.000000;0.000000;-30.000000;,
  -20.000000;40.000000;20.000000;,
  20.000000;40.000000;10.000000;,
  20.000000;40.000000;-9.999998;,
  9.999998;40.000000;-20.000000;,
  -10.000000;40.000000;-20.000000;,
  -20.000000;40.000000;10.000000;,
  -20.000000;40.000000;-10.000001;,
  -10.000000;40.000000;20.000000;,
  10.000001;40.000000;20.000000;;
  66;
  3;21,23,22;,
  3;23,21,20;,
  3;0,5,1;,
  3;0,19,5;,
  3;0,12,19;,
  3;0,4,12;,
  3;36,7,3;,
  3;36,8,7;,
  3;36,10,8;,
  3;36,37,10;,
  3;38,6,2;,
  3;38,13,6;,
  3;38,16,13;,
  3;38,39,16;,
  3;40,41,42;,
  3;40,11,41;,
  3;40,9,11;,
  3;40,43,9;,
  3;24,26,25;,
  3;26,24,27;,
  3;44,18,17;,
  3;18,44,45;,
  3;46,15,18;,
  3;15,46,47;,
  3;28,30,29;,
  3;30,28,31;,
  3;48,15,49;,
  3;15,48,14;,
  3;50,17,14;,
  3;17,50,51;,
  3;14,18,15;,
  3;18,14,17;,
  3;32,34,33;,
  3;34,32,35;,
  3;4,21,12;,
  3;21,4,20;,
  3;47,22,15;,
  3;22,47,21;,
  3;15,23,49;,
  3;23,15,22;,
  3;11,20,41;,
  3;20,11,23;,
  3;39,25,16;,
  3;25,39,24;,
  3;51,26,17;,
  3;26,51,25;,
  3;17,27,44;,
  3;27,17,26;,
  3;8,24,7;,
  3;24,8,27;,
  3;43,29,9;,
  3;29,43,28;,
  3;48,30,14;,
  3;30,48,29;,
  3;14,31,50;,
  3;31,14,30;,
  3;13,28,6;,
  3;28,13,31;,
  3;37,33,10;,
  3;33,37,32;,
  3;45,34,18;,
  3;34,45,33;,
  3;18,35,46;,
  3;35,18,34;,
  3;19,32,5;,
  3;32,19,35;;

  MeshNormals  {
   11;
   0.000000;1.000000;0.000000;,
   0.000000;0.242536;-0.970142;,
   0.970142;0.242536;0.000000;,
   0.000000;0.242536;0.970142;,
   -0.970142;0.242536;0.000000;,
   0.000000;0.000000;-1.000000;,
   1.000000;0.000000;0.000000;,
   -0.000000;0.000000;1.000000;,
   -1.000000;0.000000;0.000000;,
   0.000000;0.000000;1.000000;,
   -1.000000;0.000000;0.000000;;
   66;
   3;0,0,0;,
   3;0,0,0;,
   3;1,1,1;,
   3;1,1,1;,
   3;1,1,1;,
   3;1,1,1;,
   3;2,2,2;,
   3;2,2,2;,
   3;2,2,2;,
   3;2,2,2;,
   3;3,3,3;,
   3;3,3,3;,
   3;3,3,3;,
   3;3,3,3;,
   3;4,4,4;,
   3;4,4,4;,
   3;4,4,4;,
   3;4,4,4;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;0,0,0;,
   3;5,5,5;,
   3;5,5,5;,
   3;6,6,6;,
   3;6,6,6;,
   3;7,7,7;,
   3;7,7,7;,
   3;8,8,8;,
   3;8,8,8;,
   3;9,9,9;,
   3;9,9,9;,
   3;10,10,10;,
   3;10,10,10;,
   3;5,5,5;,
   3;5,5,5;,
   3;6,6,6;,
   3;6,6,6;,
   3;8,8,8;,
   3;8,8,8;,
   3;5,5,5;,
   3;5,5,5;,
   3;6,6,6;,
   3;6,6,6;,
   3;9,9,9;,
   3;9,9,9;,
   3;6,6,6;,
   3;6,6,6;,
   3;9,9,9;,
   3;9,9,9;,
   3;8,8,8;,
   3;8,8,8;,
   3;5,5,5;,
   3;5,5,5;;
  }

  MeshMaterialList  {
   1;
   66;
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0;
   { PDX01_-_Default }
  }

  MeshTextureCoords  {
   52;
   0.000000;1.000000;,
   1.000000;1.000000;,
   1.000000;1.000000;,
   1.000000;1.000000;,
   0.000000;0.000000;,
   1.000000;0.000000;,
   1.000000;0.000000;,
   1.000000;0.000000;,
   0.750000;0.000000;,
   0.250000;0.000000;,
   0.250000;0.000000;,
   0.750000;0.000000;,
   0.250000;0.000000;,
   0.750000;0.000000;,
   0.250000;0.250000;,
   0.250000;0.750000;,
   0.250000;0.000000;,
   0.750000;0.250000;,
   0.750000;0.750000;,
   0.750000;0.000000;,
   0.000000;1.000000;,
   0.250000;1.000000;,
   0.250000;0.750000;,
   0.000000;0.750000;,
   1.000000;0.000000;,
   0.750000;0.000000;,
   0.750000;0.250000;,
   1.000000;0.250000;,
   0.000000;0.000000;,
   0.000000;0.250000;,
   0.250000;0.250000;,
   0.250000;0.000000;,
   1.000000;1.000000;,
   1.000000;0.750000;,
   0.750000;0.750000;,
   0.750000;1.000000;,
   0.000000;1.000000;,
   0.000000;0.000000;,
   0.000000;1.000000;,
   0.000000;0.000000;,
   0.000000;1.000000;,
   1.000000;0.000000;,
   1.000000;1.000000;,
   0.000000;0.000000;,
   1.000000;0.250000;,
   1.000000;0.750000;,
   0.750000;1.000000;,
   0.250000;1.000000;,
   0.000000;0.250000;,
   0.000000;0.750000;,
   0.250000;0.000000;,
   0.750000;0.000000;;
  }
 }
}