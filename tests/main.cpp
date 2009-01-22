int WINAPI WinMain(HINSTANCE hinstance,
HINSTANCE prevInstance,
PSTR cmdLine,
int showCmd)
{
if(!d3d::InitD3D(hinstance,
800, 600, true, D3DDEVTYPE_HAL, &Device))
{
::MessageBox(0, "InitD3D() - FAILED", 0, 0);
return 0;
}
if(!Setup())
{
::MessageBox(0, "Setup() - FAILED", 0, 0);
return 0;
}
d3d::EnterMsgLoop( Display );
Cleanup();
Device->Release();