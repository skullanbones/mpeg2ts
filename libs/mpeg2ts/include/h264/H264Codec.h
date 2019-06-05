#pragma once


#include <memory>
#include <vector>
#include <string>


namespace h264 {

enum class H264InfoType
{
    Info,
    SliceHeader,
    SequenceParameterSet,
    PictureParameterSet
};


struct EsInfoH264SliceHeader
{
    int sliceType{ 0 };
    std::string sliceTypeStr{ "" };
    int ppsId{ 0 };
    bool field{ false }; // or frame
    bool top{ false };   // or bottom
    int frame_num{ 0 };
};


struct EsInfoH264SequenceParameterSet
{
    int profileIdc{ 0 };
    int levelIdc{ 0 };
    int spsId{ 0 };
    int lumaBits{ 0 };
    int chromaBits{ 0 };
    int numRefPics{ 0 };
    int width{ 0 };
    int height{ 0 };
};


struct EsInfoH264PictureParameterSet
{
    int ppsId{ 0 };
    int spsId{ 0 };
};


/// ITU-T Rec.H H.264 Table 7-1 – NAL unit type codes
enum class NalUnitType
{
    Unspecified = 0,
    Coded_slice_of_a_non_IDR_picture = 1,
    Coded_slice_data_partition_A = 2,
    Coded_slice_data_partition_B = 3,
    Coded_slice_data_partition_C = 4,
    Coded_slice_of_an_IDR_picture = 5,
    Supplemental_enhancement_information_SEI = 6,
    Sequence_parameter_set = 7,
    Picture_parameter_set = 8,
    Access_unit_delimiter = 9,
    End_of_sequence = 10,
    End_of_stream = 11,
    Filler_data = 12
};


struct EsInfoH264
{
    H264InfoType type;
    NalUnitType nalUnitType{ NalUnitType::Unspecified };
    std::string msg{ "" };
    EsInfoH264SliceHeader slice;
    EsInfoH264SequenceParameterSet sps;
    EsInfoH264PictureParameterSet pps;
};

class H264EsParser;
class H264Codec
{
public:
    H264Codec();
    ~H264Codec();

    /*!
     * @brief Parses a binary buffer containing H264 codec data and
     * let the specialization analyze the results.
     * @param buf The binary data to parse
     * @return H264 parsed metadata
     */
    std::vector<EsInfoH264> parse(const std::vector<uint8_t>& buf);

    /*!
     * @brief Analyze the content on data after startcodes.
     */
    std::vector<EsInfoH264> analyze();

    /*!
     * @brief Convert H264InfoType to string
     * @return H264InfoType string
     */
    std::string toString(H264InfoType e);

    /*!
     * @brief Convert NalUnitType to string
     * @return NalUnitType string
     */
    std::string toString(NalUnitType e);
private:
    // Non copyable
    H264Codec(const H264Codec&);
    H264Codec& operator=(const H264Codec&);
    // Non movable
    H264Codec(H264Codec&&);
    H264Codec& operator=(H264Codec&&);

    std::unique_ptr<H264EsParser> mPimpl;
};


} // namespace h264