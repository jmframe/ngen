#ifndef NGEN_BMI_C_FORMULATION_TEST_CPP
#define NGEN_BMI_C_FORMULATION_TEST_CPP

#ifdef NGEN_BMI_C_LIB_TESTS_ACTIVE

#include "Bmi_Formulation.hpp"
#include "Bmi_C_Formulation.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "FileChecker.h"
#include "Formulation_Manager.hpp"
#include "Forcing.h"

using namespace realization;

class Bmi_C_Formulation_Test : public ::testing::Test {
protected:

    static std::string find_file(std::vector<std::string> dir_opts, const std::string& basename) {
        std::vector<std::string> file_opts(dir_opts.size());
        for (int i = 0; i < dir_opts.size(); ++i)
            file_opts[i] = dir_opts[i] + basename;
        return utils::FileChecker::find_first_readable(file_opts);
    }

    static std::string get_friend_bmi_init_config(const Bmi_C_Formulation& formulation) {
        return formulation.get_bmi_init_config();
    }

    static std::string get_friend_bmi_main_output_var(const Bmi_C_Formulation& formulation) {
        return formulation.get_bmi_main_output_var();
    }

    static std::string get_friend_forcing_file_path(const Bmi_C_Formulation& formulation) {
        return formulation.get_forcing_file_path();
    }

    static bool get_friend_is_bmi_using_forcing_file(const Bmi_C_Formulation& formulation) {
        return formulation.is_bmi_using_forcing_file();
    }

    static std::string get_friend_model_type_name(Bmi_C_Formulation& formulation) {
        return formulation.get_model_type_name();
    }

    static double get_friend_var_value_as_double(Bmi_C_Formulation& formulation, const string& var_name) {
        return formulation.get_var_value_as_double(var_name);
    }

    void SetUp() override;

    void TearDown() override;

    double parse_from_delimited_string(const std::string& str_val, const std::string& delimiter, unsigned int index) {
        std::stringstream str_stream(str_val);

        unsigned int i = 0;
        while (str_stream.good()) {
            std::string substring;
            std::getline(str_stream, substring, delimiter.c_str()[0]);
            if (i++ == index)
                return std::stod(substring);
        }

        throw std::runtime_error("Cannot parse " + std::to_string(index) +
                                 "-th substring that exceeds number of substrings in string [" + str_val + "] (" +
                                 std::to_string(i) + ").");
    }

    std::vector<std::string> forcing_dir_opts;
    std::vector<std::string> bmi_init_cfg_dir_opts;

    std::vector<std::string> config_json;
    std::vector<std::string> catchment_ids;
    std::vector<std::string> model_type_name;
    std::vector<std::string> forcing_file;
    std::vector<std::string> init_config;
    std::vector<std::string> main_output_variable;
    std::vector<bool> uses_forcing_file;
    std::vector<std::shared_ptr<forcing_params>> forcing_params_examples;
    std::vector<geojson::GeoJSON> config_properties;
    std::vector<boost::property_tree::ptree> config_prop_ptree;

};

void Bmi_C_Formulation_Test::SetUp() {
    testing::Test::SetUp();

#define EX_COUNT 2

    forcing_dir_opts = {"./data/forcing/", "../data/forcing/", "../../data/forcing/"};
    bmi_init_cfg_dir_opts = {"./test/data/bmi/c/cfe/", "../test/data/bmi/c/cfe/", "../../test/data/bmi/c/cfe/"};

    config_json = std::vector<std::string>(EX_COUNT);
    catchment_ids = std::vector<std::string>(EX_COUNT);
    model_type_name = std::vector<std::string>(EX_COUNT);
    forcing_file = std::vector<std::string>(EX_COUNT);
    init_config = std::vector<std::string>(EX_COUNT);
    main_output_variable  = std::vector<std::string>(EX_COUNT);
    uses_forcing_file = std::vector<bool>(EX_COUNT);
    forcing_params_examples = std::vector<std::shared_ptr<forcing_params>>(EX_COUNT);
    config_properties = std::vector<geojson::GeoJSON>(EX_COUNT);
    config_prop_ptree = std::vector<boost::property_tree::ptree>(EX_COUNT);

    /* Set up the basic/explicit example index details in the arrays */
    catchment_ids[0] = "cat-87";
    model_type_name[0] = "bmi_c_cfe";
    forcing_file[0] = find_file(forcing_dir_opts, "cat-87_2015-12-01 00_00_00_2015-12-30 23_00_00.csv");
    init_config[0] = find_file(bmi_init_cfg_dir_opts, "cat_87_bmi_config.txt");
    main_output_variable[0] = "Q_OUT";
    uses_forcing_file[0] = true;

    catchment_ids[1] = "cat-87";
    model_type_name[1] = "bmi_c_cfe";
    forcing_file[1] = find_file(forcing_dir_opts, "cat-87_2015-12-01 00_00_00_2015-12-30 23_00_00.csv");
    init_config[1] = find_file(bmi_init_cfg_dir_opts, "cat_87_bmi_config.txt");
    main_output_variable[1] = "Q_OUT";
    uses_forcing_file[1] = true;

    std::string variables_with_rain_rate = "                \"output_variables\": [\"RAIN_RATE\",\n"
                                           "                    \"SCHAAKE_OUTPUT_RUNOFF\",\n"
                                           "                    \"GIUH_RUNOFF\",\n"
                                           "                    \"NASH_LATERAL_RUNOFF\",\n"
                                           "                    \"DEEP_GW_TO_CHANNEL_FLUX\",\n"
                                           "                    \"Q_OUT\"],\n";

    /* Set up the derived example details */
    for (int i = 0; i < EX_COUNT; i++) {
        std::shared_ptr<forcing_params> params = std::make_shared<forcing_params>(
                forcing_params(forcing_file[i], "2015-12-01 00:00:00", "2015-12-01 23:00:00"));
        std::string variables_line = (i == 1) ? variables_with_rain_rate : "";
        forcing_params_examples[i] = params;
        config_json[i] = "{"
                         "    \"global\": {},"
                         "    \"catchments\": {"
                         "        \"" + catchment_ids[i] + "\": {"
                         "            \"bmi_c\": {"
                         "                \"model_type_name\": \"" + model_type_name[i] + "\","
                         "                \"forcing_file\": \"" + forcing_file[i] + "\","
                         "                \"init_config\": \"" + init_config[i] + "\","
                         "                \"main_output_variable\": \"" + main_output_variable[i] + "\","
                         + variables_line +
                         "                \"uses_forcing_file\": " + (uses_forcing_file[i] ? "true" : "false") + ""
                         "            },"
                         "            \"forcing\": { \"path\": \"" + forcing_file[i] + "\"}"
                         "        }"
                         "    }"
                         "}";

        std::stringstream stream;
        stream << config_json[i];

        boost::property_tree::ptree loaded_tree;
        boost::property_tree::json_parser::read_json(stream, loaded_tree);
        config_prop_ptree[i] = loaded_tree.get_child("catchments").get_child(catchment_ids[i]).get_child("bmi_c");
    }
}

void Bmi_C_Formulation_Test::TearDown() {
    Test::TearDown();
}

/** Simple test to make sure the model initializes. */
TEST_F(Bmi_C_Formulation_Test, Initialize_0_a) {
    int ex_index = 0;

    Bmi_C_Formulation formulation(catchment_ids[ex_index], *forcing_params_examples[ex_index], utils::StreamHandler());
    formulation.create_formulation(config_prop_ptree[ex_index]);

    ASSERT_EQ(get_friend_model_type_name(formulation), model_type_name[ex_index]);
    ASSERT_EQ(get_friend_forcing_file_path(formulation), forcing_file[ex_index]);
    ASSERT_EQ(get_friend_bmi_init_config(formulation), init_config[ex_index]);
    ASSERT_EQ(get_friend_bmi_main_output_var(formulation), main_output_variable[ex_index]);
    ASSERT_EQ(get_friend_is_bmi_using_forcing_file(formulation), uses_forcing_file[ex_index]);
}

/** Simple test of get response. */
TEST_F(Bmi_C_Formulation_Test, GetResponse_0_a) {
    int ex_index = 0;

    Bmi_C_Formulation formulation(catchment_ids[ex_index], *forcing_params_examples[ex_index], utils::StreamHandler());
    formulation.create_formulation(config_prop_ptree[ex_index]);

    double response = formulation.get_response(0, 3600);
    // TODO: val seems to be this for now ... do something but account for error bound
    ASSERT_EQ(response, 0.19108623197892585);
}

/** Test of get response after several iterations. */
TEST_F(Bmi_C_Formulation_Test, GetResponse_0_b) {
    int ex_index = 0;

    Bmi_C_Formulation formulation(catchment_ids[ex_index], *forcing_params_examples[ex_index], utils::StreamHandler());
    formulation.create_formulation(config_prop_ptree[ex_index]);

    double response;
    for (int i = 0; i < 720; i++) {
        response = formulation.get_response(i, 3600);
    }
    // TODO: val seems to be this for now ... do something but account for error bound
    ASSERT_EQ(response, 0.0025434016922131421);
}

/** Simple test of output. */
TEST_F(Bmi_C_Formulation_Test, GetOutputLineForTimestep_0_a) {
    int ex_index = 0;

    Bmi_C_Formulation formulation(catchment_ids[ex_index], *forcing_params_examples[ex_index], utils::StreamHandler());
    formulation.create_formulation(config_prop_ptree[ex_index]);

    double response = formulation.get_response(0, 3600);
    std::string output = formulation.get_output_line_for_timestep(0, ",");
    ASSERT_EQ(output, "0.000000,0.000000,0.000000,0.000000,0.191086,0.191086");
}

/** Simple test of output with modified variables. */
TEST_F(Bmi_C_Formulation_Test, GetOutputLineForTimestep_1_a) {
    int ex_index = 1;

    Bmi_C_Formulation formulation(catchment_ids[ex_index], *forcing_params_examples[ex_index], utils::StreamHandler());
    formulation.create_formulation(config_prop_ptree[ex_index]);

    double response = formulation.get_response(0, 3600);
    std::string output = formulation.get_output_line_for_timestep(0, ",");
    ASSERT_EQ(output, "0.000000,0.000000,0.000000,0.000000,0.191086,0.191086");
}

/** Simple test of output with modified variables, picking time step when there was non-zero rain rate. */
TEST_F(Bmi_C_Formulation_Test, GetOutputLineForTimestep_1_b) {
    int ex_index = 1;

    Bmi_C_Formulation formulation(catchment_ids[ex_index], *forcing_params_examples[ex_index], utils::StreamHandler());
    formulation.create_formulation(config_prop_ptree[ex_index]);

    int i = 0;
    while (i < 542)
        formulation.get_response(i++, 3600);
    double response = formulation.get_response(i, 3600);
    std::string output = formulation.get_output_line_for_timestep(i, ",");
    ASSERT_EQ(output, "0.003153,0.004346,0.000449,0.001001,0.001424,0.002874");
}



#endif  // NGEN_BMI_C_LIB_TESTS_ACTIVE

#endif NGEN_BMI_C_FORMULATION_TEST_CPP