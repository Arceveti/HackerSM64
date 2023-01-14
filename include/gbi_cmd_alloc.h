#pragma once

#define GFX_ALLOC(x) _ALLOC_CMD_##x

#define _ALLOC_CMD_gDma0p						1
#define _ALLOC_CMD_gsDma0p						1

#define _ALLOC_CMD_gDma1p						1
#define _ALLOC_CMD_gsDma1p						1

#define _ALLOC_CMD_gDma2p						1
#define _ALLOC_CMD_gsDma2p						1

#define _ALLOC_CMD_gSPNoOp						1
#define _ALLOC_CMD_gsSPNoOp						1

#define _ALLOC_CMD_gSPMatrix					1
#define _ALLOC_CMD_gsSPMatrix					1

#define _ALLOC_CMD_gSPVertex					1
#define _ALLOC_CMD_gsSPVertex					1

#define _ALLOC_CMD_gSPViewport					1
#define _ALLOC_CMD_gsSPViewport					1

#define _ALLOC_CMD_gSPDisplayList				1
#define _ALLOC_CMD_gsSPDisplayList				1

#define _ALLOC_CMD_gSPBranchList				1
#define _ALLOC_CMD_gsSPBranchList				1

#define _ALLOC_CMD_gSPSprite2DBase				1
#define _ALLOC_CMD_gsSPSprite2DBase				1

#define _ALLOC_CMD_gImmp0						1
#define _ALLOC_CMD_gsImmp0						1

#define _ALLOC_CMD_gImmp1						1
#define _ALLOC_CMD_gsImmp1						1

#define _ALLOC_CMD_gImmp2						1
#define _ALLOC_CMD_gsImmp2						1

#define _ALLOC_CMD_gImmp3						1
#define _ALLOC_CMD_gsImmp3						1

#define _ALLOC_CMD_gImmp21						1
#define _ALLOC_CMD_gsImmp21						1

#define _ALLOC_CMD_gMoveWd						1
#define _ALLOC_CMD_gsMoveWd						1

#define _ALLOC_CMD_gSPSprite2DScaleFlip			1
#define _ALLOC_CMD_gsSPSprite2DScaleFlip		1

#define _ALLOC_CMD_gSPSprite2DDraw				1
#define _ALLOC_CMD_gsSPSprite2DDraw				1

#define _ALLOC_CMD___gsSP1Triangle_w1			1
#define _ALLOC_CMD___gsSP1Triangle_w1f			1

#define _ALLOC_CMD___gsSPLine3D_w1				1
#define _ALLOC_CMD___gsSPLine3D_w1f				1

#define _ALLOC_CMD___gsSP1Quadrangle_w1f		1
#define _ALLOC_CMD___gsSP1Quadrangle_w2f		1

#define _ALLOC_CMD_gSP1Triangle					1
#define _ALLOC_CMD_gsSP1Triangle				1

#define _ALLOC_CMD_gSPLine3D					1
#define _ALLOC_CMD_gsSPLine3D					1

#define _ALLOC_CMD_gSPLineW3D					1
#define _ALLOC_CMD_gsSPLineW3D					1

#define _ALLOC_CMD_gSP1Quadrangle				1
#define _ALLOC_CMD_gsSP1Quadrangle				1

#define _ALLOC_CMD_gSP2Triangles				1
#define _ALLOC_CMD_gsSP2Triangles				1

#define _ALLOC_CMD_gSPCullDisplayList			1
#define _ALLOC_CMD_gsSPCullDisplayList			1

#define _ALLOC_CMD_gSPSegment					1
#define _ALLOC_CMD_gsSPSegment					1

#define _ALLOC_CMD_gSPClipRatio					4
#define _ALLOC_CMD_gsSPClipRatio				4

#ifdef F3DEX_GBI_2
	#define _ALLOC_CMD_gSPInsertMatrix				0
	#define _ALLOC_CMD_gsSPInsertMatrix				0
#else
	#define _ALLOC_CMD_gSPInsertMatrix				1
	#define _ALLOC_CMD_gsSPInsertMatrix				1
#endif

#ifdef F3DEX_GBI_2
	#define _ALLOC_CMD_gSPForceMatrix				2
	#define _ALLOC_CMD_gsSPForceMatrix				2
#else
	#define _ALLOC_CMD_gSPForceMatrix				4
	#define _ALLOC_CMD_gsSPForceMatrix				4
#endif

#define _ALLOC_CMD_gSPModifyVertex				1
#define _ALLOC_CMD_gsSPModifyVertex				1

#define _ALLOC_CMD_gSPBranchLessZrg				2
#define _ALLOC_CMD_gsSPBranchLessZrg			2

#define _ALLOC_CMD_gSPBranchLessZ				2
#define _ALLOC_CMD_gsSPBranchLessZ				2

#define _ALLOC_CMD_gSPBranchLessZraw			2
#define _ALLOC_CMD_gsSPBranchLessZraw			2

#define _ALLOC_CMD_gSPLoadUcodeEx				2
#define _ALLOC_CMD_gsSPLoadUcodeEx				2

#define _ALLOC_CMD_gSPLoadUcode					2
#define _ALLOC_CMD_gsSPLoadUcode				2

#define _ALLOC_CMD_gSPLoadUcodeL				2
#define _ALLOC_CMD_gsSPLoadUcodeL				2

#define _ALLOC_CMD_gSPDma_io					1
#define _ALLOC_CMD_gsSPDma_io					1

#define _ALLOC_CMD_gSPDmaRead					1
#define _ALLOC_CMD_gsSPDmaRead					1
#define _ALLOC_CMD_gSPDmaWrite					1
#define _ALLOC_CMD_gsSPDmaWrite					1

#define _ALLOC_CMD_gSPNumLights					1
#define _ALLOC_CMD_gsSPNumLights				1

#define _ALLOC_CMD_gSPLight						1
#define _ALLOC_CMD_gsSPLight					1

#define _ALLOC_CMD_gSPLightColor				2
#define _ALLOC_CMD_gsSPLightColor				2

#define _ALLOC_CMD_gSPSetLights0				3
#define _ALLOC_CMD_gsSPSetLights0				3

#define _ALLOC_CMD_gSPSetLights1				3
#define _ALLOC_CMD_gsSPSetLights1				3

#define _ALLOC_CMD_gSPSetLights2				4
#define _ALLOC_CMD_gsSPSetLights2				4

#define _ALLOC_CMD_gSPSetLights3				5
#define _ALLOC_CMD_gsSPSetLights3				5

#define _ALLOC_CMD_gSPSetLights4				6
#define _ALLOC_CMD_gsSPSetLights4				6

#define _ALLOC_CMD_gSPSetLights5				7
#define _ALLOC_CMD_gsSPSetLights5				7

#define _ALLOC_CMD_gSPSetLights6				8
#define _ALLOC_CMD_gsSPSetLights6				8

#define _ALLOC_CMD_gSPSetLights7				9
#define _ALLOC_CMD_gsSPSetLights7				9

#define _ALLOC_CMD_gSPLookAtX					1
#define _ALLOC_CMD_gsSPLookAtX					1

#define _ALLOC_CMD_gSPLookAtY					1
#define _ALLOC_CMD_gsSPLookAtY					1

#define _ALLOC_CMD_gSPLookAt					2
#define _ALLOC_CMD_gsSPLookAt					2

#define _ALLOC_CMD_gDPSetHilite1Tile			1
#define _ALLOC_CMD_gDPSetHilite2Tile			1

#define _ALLOC_CMD_gSPFogFactor					1
#define _ALLOC_CMD_gsSPFogFactor				1

#define _ALLOC_CMD_gSPFogPosition				1
#define _ALLOC_CMD_gsSPFogPosition				1

#define _ALLOC_CMD_gSPTexture					1
#define _ALLOC_CMD_gsSPTexture					1

#define _ALLOC_CMD_gSPTextureL					1
#define _ALLOC_CMD_gsSPTextureL					1

#define _ALLOC_CMD_gSPPerspNormalize			1
#define _ALLOC_CMD_gsSPPerspNormalize			1

#define _ALLOC_CMD_gSPPopMatrixN				1
#define _ALLOC_CMD_gsSPPopMatrixN				1

#define _ALLOC_CMD_gSPPopMatrix					1
#define _ALLOC_CMD_gsSPPopMatrix				1

#define _ALLOC_CMD_gSPEndDisplayList			1
#define _ALLOC_CMD_gsSPEndDisplayList			1

#define _ALLOC_CMD_gSPGeometryMode				1

#ifdef F3DEX_GBI_2
	#define _ALLOC_CMD_gsSPGeometryMode				1
	#define _ALLOC_CMD_gsSPGeometryModeSetFirst		1
#else
	#define _ALLOC_CMD_gsSPGeometryMode				2
	#define _ALLOC_CMD_gsSPGeometryModeSetFirst		2
#endif

#define _ALLOC_CMD_gSPSetGeometryMode			1
#define _ALLOC_CMD_gsSPSetGeometryMode			1

#define _ALLOC_CMD_gSPClearGeometryMode			1
#define _ALLOC_CMD_gsSPClearGeometryMode		1

#define _ALLOC_CMD_gSPLoadGeometryMode			1
#define _ALLOC_CMD_gsSPLoadGeometryMode			1

#define _ALLOC_CMD_gSPSetOtherMode				1
#define _ALLOC_CMD_gsSPSetOtherMode				1

#define _ALLOC_CMD_gDPPipelineMode				1
#define _ALLOC_CMD_gsDPPipelineMode				1

#define _ALLOC_CMD_gDPSetCycleType				1
#define _ALLOC_CMD_gsDPSetCycleType				1

#define _ALLOC_CMD_gDPSetTexturePersp			1
#define _ALLOC_CMD_gsDPSetTexturePersp			1

#define _ALLOC_CMD_gDPSetTextureDetail			1
#define _ALLOC_CMD_gsDPSetTextureDetail			1

#define _ALLOC_CMD_gDPSetTextureLOD				1
#define _ALLOC_CMD_gsDPSetTextureLOD			1

#define _ALLOC_CMD_gDPSetTextureLUT				1
#define _ALLOC_CMD_gsDPSetTextureLUT			1

#define _ALLOC_CMD_gDPSetTextureFilter			1
#define _ALLOC_CMD_gsDPSetTextureFilter			1

#define _ALLOC_CMD_gDPSetTextureConvert			1
#define _ALLOC_CMD_gsDPSetTextureConvert		1

#define _ALLOC_CMD_gDPSetCombineKey				1
#define _ALLOC_CMD_gsDPSetCombineKey			1

#define _ALLOC_CMD_gDPSetColorDither			1
#define _ALLOC_CMD_gsDPSetColorDither			1

#define _ALLOC_CMD_gDPSetAlphaDither			1
#define _ALLOC_CMD_gsDPSetAlphaDither			1

#define _ALLOC_CMD_gDPSetBlendMask				1
#define _ALLOC_CMD_gsDPSetBlendMask				1

#define _ALLOC_CMD_gDPSetAlphaCompare			1
#define _ALLOC_CMD_gsDPSetAlphaCompare			1

#define _ALLOC_CMD_gDPSetDepthSource			1
#define _ALLOC_CMD_gsDPSetDepthSource			1

#define _ALLOC_CMD_gDPSetRenderMode				1
#define _ALLOC_CMD_gsDPSetRenderMode			1

#define _ALLOC_CMD_gSetImage					1
#define _ALLOC_CMD_gsSetImage					1

#define _ALLOC_CMD_gDPSetColorImage				1
#define _ALLOC_CMD_gsDPSetColorImage			1

#define _ALLOC_CMD_gDPSetDepthImage				1
#define _ALLOC_CMD_gsDPSetDepthImage			1

#define _ALLOC_CMD_gDPSetMaskImage				1
#define _ALLOC_CMD_gsDPSetMaskImage				1

#define _ALLOC_CMD_gDPSetTextureImage			1
#define _ALLOC_CMD_gsDPSetTextureImage			1

#define _ALLOC_CMD_gDPSetCombine				1
#define _ALLOC_CMD_gsDPSetCombine				1

#define _ALLOC_CMD_gCCc0w0						1
#define _ALLOC_CMD_gCCc1w0						1
#define _ALLOC_CMD_gCCc0w1						1
#define _ALLOC_CMD_gCCc1w1						1

#define _ALLOC_CMD_gDPSetCombineLERP			1
#define _ALLOC_CMD_gsDPSetCombineLERP			1

#define _ALLOC_CMD_gDPSetCombineMode			1
#define _ALLOC_CMD_gsDPSetCombineMode			1

#define _ALLOC_CMD_gDPSetColor					1
#define _ALLOC_CMD_gsDPSetColor					1

#define _ALLOC_CMD_DPRGBColor					1
#define _ALLOC_CMD_sDPRGBColor					1

#define _ALLOC_CMD_gDPSetEnvColor				1
#define _ALLOC_CMD_gsDPSetEnvColor				1

#define _ALLOC_CMD_gDPSetBlendColor				1
#define _ALLOC_CMD_gsDPSetBlendColor			1

#define _ALLOC_CMD_gDPSetFogColor				1
#define _ALLOC_CMD_gsDPSetFogColor				1

#define _ALLOC_CMD_gDPSetFillColor				1
#define _ALLOC_CMD_gsDPSetFillColor				1

#define _ALLOC_CMD_gDPSetPrimDepth				1
#define _ALLOC_CMD_gsDPSetPrimDepth				1

#define _ALLOC_CMD_gDPSetPrimColor				1
#define _ALLOC_CMD_gsDPSetPrimColor				1

#define _ALLOC_CMD_gDPSetOtherMode				1
#define _ALLOC_CMD_gsDPSetOtherMode				1

#define _ALLOC_CMD_gDPLoadTileGeneric			1
#define _ALLOC_CMD_gsDPLoadTileGeneric			1

#define _ALLOC_CMD_gDPSetTileSize				1
#define _ALLOC_CMD_gsDPSetTileSize				1

#define _ALLOC_CMD_gDPLoadTile					1
#define _ALLOC_CMD_gsDPLoadTile					1

#define _ALLOC_CMD_gDPSetTile					1
#define _ALLOC_CMD_gsDPSetTile					1

#define _ALLOC_CMD_gDPLoadBlock					1
#define _ALLOC_CMD_gsDPLoadBlock				1

#define _ALLOC_CMD_gDPLoadTLUTCmd				1
#define _ALLOC_CMD_gsDPLoadTLUTCmd				1

#define _ALLOC_CMD_gDPLoadTextureBlock			7
#define _ALLOC_CMD_gDPLoadTextureBlockYuv		7
#define _ALLOC_CMD_gDPLoadTextureBlockS			7
#define _ALLOC_CMD_gDPLoadMultiBlockS			7
#define _ALLOC_CMD_gDPLoadTextureBlockYuvS		7
#define _ALLOC_CMD__gDPLoadTextureBlock			7
#define _ALLOC_CMD__gDPLoadTextureBlockTile		7
#define _ALLOC_CMD_gDPLoadMultiBlock			7
#define _ALLOC_CMD_gsDPLoadTextureBlock			7
#define _ALLOC_CMD_gsDPLoadTextureBlockS		7
#define _ALLOC_CMD__gsDPLoadTextureBlock		7
#define _ALLOC_CMD__gsDPLoadTextureBlockTile	7
#define _ALLOC_CMD_gsDPLoadMultiBlock			7
#define _ALLOC_CMD_gsDPLoadMultiBlockS			7
#define _ALLOC_CMD_gDPLoadTextureBlock_4b		7
#define _ALLOC_CMD_gDPLoadTextureBlock_4bS		7
#define _ALLOC_CMD_gDPLoadMultiBlock_4b			7
#define _ALLOC_CMD_gDPLoadMultiBlock_4bS		7
#define _ALLOC_CMD__gDPLoadTextureBlock_4b		7
#define _ALLOC_CMD_gsDPLoadTextureBlock_4b		7
#define _ALLOC_CMD_gsDPLoadTextureBlock_4bS		7
#define _ALLOC_CMD_gsDPLoadMultiBlock_4b		7
#define _ALLOC_CMD_gsDPLoadMultiBlock_4bS		7
#define _ALLOC_CMD__gsDPLoadTextureBlock_4b		7

#ifndef _HW_VERSION_1
	#define _ALLOC_CMD_gDPLoadTextureTile			7
#else
	#define _ALLOC_CMD_gDPLoadTextureTile			_ALLOC_CMD_guDPLoadTextureTile + _ALLOC_CMD_guGetDPLoadTextureTileSz
#endif

#define _ALLOC_CMD_gDPLoadMultiTile				7
#define _ALLOC_CMD_gsDPLoadTextureTile			7
#define _ALLOC_CMD_gsDPLoadMultiTile			7
#define _ALLOC_CMD_gDPLoadTextureTile_4b		7
#define _ALLOC_CMD_gDPLoadMultiTile_4b			7
#define _ALLOC_CMD_gsDPLoadTextureTile_4b		7
#define _ALLOC_CMD_gsDPLoadMultiTile_4b			7

#ifndef _HW_VERSION_1
	#define _ALLOC_CMD_gDPLoadTLUT_pal16			6
	#define _ALLOC_CMD_gsDPLoadTLUT_pal16			6
	#define _ALLOC_CMD_gDPLoadTLUT_pal256			6
	#define _ALLOC_CMD_gsDPLoadTLUT_pal256			6
	#define _ALLOC_CMD_gDPLoadTLUT					6
	#define _ALLOC_CMD_gsDPLoadTLUT					6
#else
	#define _ALLOC_CMD_gDPLoadTLUT_pal16			7
	#define _ALLOC_CMD_gsDPLoadTLUT_pal16			7
	#define _ALLOC_CMD_gsDPLoadTLUT_pal16			7
	#define _ALLOC_CMD_gsDPLoadTLUT_pal256			7
	#define _ALLOC_CMD_gDPLoadTLUT					7
	#define _ALLOC_CMD_gsDPLoadTLUT					7
#endif

#define _ALLOC_CMD_gDPSetScissor				1
#define _ALLOC_CMD_gsDPSetScissor				1

#define _ALLOC_CMD_gDPSetScissorFrac			1
#define _ALLOC_CMD_gsDPSetScissorFrac			1

#ifdef F3DEX_GBI_2E
	#define _ALLOC_CMD_gDPFillRectangle				2
	#define _ALLOC_CMD_gsDPFillRectangle			2
#else
	#define _ALLOC_CMD_gDPFillRectangle				1
	#define _ALLOC_CMD_gsDPFillRectangle			1
#endif

#define _ALLOC_CMD_gDPScisFillRectangle			1

#define _ALLOC_CMD_gDPSetConvert				1
#define _ALLOC_CMD_gsDPSetConvert				1

#define _ALLOC_CMD_gDPSetKeyR					1
#define _ALLOC_CMD_gsDPSetKeyR					1

#define _ALLOC_CMD_gDPSetKeyGB					1
#define _ALLOC_CMD_gsDPSetKeyGB					1

#define _ALLOC_CMD_gDPNoParam					1
#define _ALLOC_CMD_gsDPNoParam					1

#define _ALLOC_CMD_gDPParam						1
#define _ALLOC_CMD_gsDPParam					1

#define _ALLOC_CMD_gsDPTextureRectangle			2
#define _ALLOC_CMD_gDPTextureRectangle			2

#define _ALLOC_CMD_gsDPTextureRectangleFlip		2
#define _ALLOC_CMD_gDPTextureRectangleFlip		2

#define _ALLOC_CMD_gSPTextureRectangle			3
#define _ALLOC_CMD_gsSPTextureRectangle			3
#define _ALLOC_CMD_gSPScisTextureRectangle		3
#define _ALLOC_CMD_gsSPTextureRectangleFlip		3
#define _ALLOC_CMD_gSPTextureRectangleFlip		3

#define _ALLOC_CMD_gsDPWord						2
#define _ALLOC_CMD_gDPWord						2

#define _ALLOC_CMD_gDPFullSync					1
#define _ALLOC_CMD_gsDPFullSync					1

#define _ALLOC_CMD_gDPTileSync					1
#define _ALLOC_CMD_gsDPTileSync					1

#define _ALLOC_CMD_gDPPipeSync					1
#define _ALLOC_CMD_gsDPPipeSync					1

#define _ALLOC_CMD_gDPLoadSync					1
#define _ALLOC_CMD_gsDPLoadSync					1

#define _ALLOC_CMD_gDPNoOp						1
#define _ALLOC_CMD_gsDPNoOp						1

#define _ALLOC_CMD_gDPNoOpTag					1
#define _ALLOC_CMD_gsDPNoOpTag					1
