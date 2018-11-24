#pragma once

static const float SceneVersion = 2.0;
//
//This is exporter to dimension3 scene format
	class Dimension3Scene : public ExportFormat 	{
    private:
        CMapWordToOb   *Meshes;
        CMapWordToOb   *Objects;
        CMapWordToOb   *Cameras;
        CMapWordToOb   *Lights;
        CMapWordToOb   *Textures;
        CMapWordToOb   *Materials;
        CMapWordToOb   *Controllers;

        MainDescriptor  SceneInformations;
	public:
	// Constructor/Destructor
										Dimension3Scene();
		virtual							~Dimension3Scene();

	// Exporter init
		override(ExportFormat)	bool	Init(bool motion);

	// Main scene info
		override(ExportFormat)	bool	SetSceneInfo		(const MainDescriptor& maininfo);

	// Export loop
		override(ExportFormat)	bool	ExportCamera		(const CameraDescriptor& camera);
		override(ExportFormat)	bool	ExportController	(const ControllerDescriptor& controller);
		override(ExportFormat)	bool	ExportHelper		(const HelperDescriptor& helper);
		override(ExportFormat)	bool	ExportLight			(const LightDescriptor& light);
		override(ExportFormat)	bool	ExportMaterial		(const MaterialDescriptor& material);
		override(ExportFormat)	bool	ExportMesh			(const MeshDescriptor& mesh);
		override(ExportFormat)	bool	ExportMotion		(const MotionDescriptor& motion);
		override(ExportFormat)	bool	ExportShape			(const ShapeDescriptor& shape);
		override(ExportFormat)	bool	ExportTexture		(const TextureDescriptor& texture);

	// End of export notification
		override(ExportFormat)	bool	EndExport			(const StatsDescriptor& stats);

        override(ExportFormat)	void    About               ();
	private:
	};
