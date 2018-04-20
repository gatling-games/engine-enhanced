#include "TextureImporter.h"

#include <crunch/inc/crnlib.h>
#include <crunch/crnlib/crn_mipmapped_texture.h>
#include <crunch/crnlib/crn_texture_conversion.h>
#include <crunch/crnlib/crn_console.h>

#include "Utils/Console.h"

bool TextureImporter::importFile(const std::string &sourceFile, const std::string &outputFile) const
{
	// Read the texture file.
	crnlib::texture_file_types::format sourceFormat = crnlib::texture_file_types::determine_file_format(sourceFile.c_str());
	crnlib::mipmapped_texture sourceTexture;
	if (!(sourceTexture.read_from_file(sourceFile.c_str(), sourceFormat)))
	{
		log("- ERROR: Failed to read source file \n");
		return false;
	}

	// Texture name determines conversion settings
    const bool isNormalMap = sourceFile.find("_normals.") != std::string::npos;
    const bool sRGB = sourceFile.find("_albedo.") != std::string::npos;
    const crnlib::texture_type type = isNormalMap ? crnlib::texture_type::cTextureTypeNormalMap : crnlib::texture_type::cTextureTypeRegularMap;

	// Dont display the crunch console output.
	// For large numbers of files it is too verbose.
	crnlib::console::disable_output();

	// Configure crunch texture compression
	crnlib::texture_conversion::convert_params settings;
	settings.m_texture_type = type;
	settings.m_pInput_texture = &sourceTexture;
	settings.m_dst_filename = outputFile.c_str();
	settings.m_dst_file_type = crnlib::texture_file_types::cFormatDDS;
	settings.m_comp_params.m_quality_level = cCRNMaxQualityLevel;
	settings.m_comp_params.m_dxt_quality = cCRNDXTQualityFast;
	settings.m_comp_params.set_flag(cCRNCompFlagPerceptual, !isNormalMap);
	settings.m_comp_params.m_num_helper_threads = 7;
	settings.m_mipmap_params.m_mode = cCRNMipModeGenerateMips;
    settings.m_mipmap_params.m_scale_mode = cCRNSMNearestPow2;
    settings.m_mipmap_params.m_gamma_filtering = sRGB;

	// Perform the texture conversion process
	// This creates a .crn file at outputFile path, which can be read at runtime.
	crnlib::texture_conversion::convert_stats stats;
	if (!process(settings, stats))
	{
		log(" - ERROR: Texture conversion failed \n");
		return false;
	}

	return true;
}