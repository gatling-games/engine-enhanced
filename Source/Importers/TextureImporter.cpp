#include "TextureImporter.h"

#include <crunch/inc/crnlib.h>
#include <crunch/crnlib/crn_mipmapped_texture.h>
#include <crunch/crnlib/crn_texture_conversion.h>
#include <crunch/crnlib/crn_console.h>

bool TextureImporter::importFile(const std::string &sourceFile, const std::string &outputFile) const
{
	// Read the texture file.
	crnlib::texture_file_types::format sourceFormat = crnlib::texture_file_types::determine_file_format(sourceFile.c_str());
	crnlib::mipmapped_texture sourceTexture;
	if (!(sourceTexture.read_from_file(sourceFile.c_str(), sourceFormat)))
	{
		printf("- ERROR: Failed to read source file \n");
		return false;
	}

	// Texture usage determines conversion settings
	const crnlib::texture_type textureType = sourceTexture.determine_texture_type();
	const bool isNormalMap = textureType == crnlib::cTextureTypeNormalMap;

	// Dont display the crunch console output.
	// For large numbers of files it is too verbose.
	crnlib::console::disable_output();

	// Configure crunch texture compression
	crnlib::texture_conversion::convert_params settings;
	settings.m_texture_type = textureType;
	settings.m_pInput_texture = &sourceTexture;
	settings.m_dst_filename = outputFile.c_str();
	settings.m_dst_file_type = crnlib::texture_file_types::cFormatDDS;
	settings.m_comp_params.m_quality_level = cCRNMaxQualityLevel;
	settings.m_comp_params.m_dxt_quality = cCRNDXTQualityUber;
	settings.m_comp_params.set_flag(cCRNCompFlagPerceptual, !isNormalMap);
	settings.m_comp_params.m_num_helper_threads = 7;
	settings.m_mipmap_params.m_mode = cCRNMipModeGenerateMips;
    settings.m_mipmap_params.m_scale_mode = cCRNSMNearestPow2;

	// Detect normal map textures from their file name.
	// The filename ends in _normals, _normal, or _n
	if(sourceFile.find("_normals.") != std::string::npos
		|| sourceFile.find("_normal.") != std::string::npos
		|| sourceFile.find("_n.") != std::string::npos)
	{
		settings.m_texture_type = crnlib::cTextureTypeNormalMap;
	}

	// Perform the texture conversion process
	// This creates a .crn file at outputFile path, which can be read at runtime.
	crnlib::texture_conversion::convert_stats stats;
	if (!process(settings, stats))
	{
		printf(" - ERROR: Texture conversion failed \n");
		return false;
	}

	return true;
}