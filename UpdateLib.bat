// 명령어			옵션		원본 파일의 위치					사본 파일을 저장할 위치
	

xcopy			/y/s	.\Engine\Public\*.*							.\EngineSDK\Inc\
xcopy			/y		.\Engine\Bin\Engine.lib						.\EngineSDK\Lib\

xcopy			/y		.\Engine\Bin\Engine.dll						.\Client\Bin\
xcopy			/y		.\Engine\Bin\Engine.dll						.\Tool\Bin\
xcopy			/y		.\Engine\Bin\Engine.dll						.\Map_Tool\Bin\
xcopy			/y		.\Engine\Bin\Engine.dll						.\Class\Bin\

xcopy			/y		.\Engine\Bin\assimp-vc143-mt.dll			.\Client\Bin\
xcopy			/y		.\Engine\Bin\assimp-vc143-mt.dll			.\Tool\Bin\
xcopy			/y		.\Engine\Bin\assimp-vc143-mt.dll			.\Map_Tool\Bin\
xcopy			/y		.\Engine\Bin\assimp-vc143-mt.dll			.\Class\Bin\


xcopy			/y		.\Engine\Bin\assimp-vc143-mtd.dll			.\Client\Bin\
xcopy			/y		.\Engine\Bin\assimp-vc143-mtd.dll			.\Tool\Bin\
xcopy			/y		.\Engine\Bin\assimp-vc143-mtd.dll			.\Map_Tool\Bin\
xcopy			/y		.\Engine\Bin\assimp-vc143-mtd.dll			.\Class\Bin\


xcopy			/y/s	.\Engine\Bin\ShaderFiles\*.*		.\EngineSDK\hlsl\

