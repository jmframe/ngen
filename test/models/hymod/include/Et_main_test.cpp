#include "gtest/gtest.h"
#include <stdio.h>
#include "kernels/evapotranspiration/Et_main.hpp"

class EtMainTest : public ::testing::Test {

    protected:

    EtMainTest() {

    }

    ~EtMainTest() override {

    }

    void SetUp() override;

    void TearDown() override;

    void setupArbitraryExampleCase();

};

void EtMainTest::SetUp() {
    setupArbitraryExampleCase();
}

void EtMainTest::TearDown() {

}

void EtMainTest::setupArbitraryExampleCase() {

}


TEST_F(EtMainTest, TestEnergyBalanceMethod)
{
  double et_m_per_s;
  int et_method_option;
  //et_method_option = 1;    use_energy_balance_method
  //et_method_option = 2;    use_aerodynamic_method
  //et_method_option = 3;    use_combination_method
  //et_method_option = 4;    use_priestley_taylor_method
  //et_method_option = 5;    use_penman_monteith_method
  et_method_option = 1;
  et_m_per_s = run_et_main(et_method_option);

  //EXPECT_DOUBLE_EQ (8.594743e-08, et_m_per_s);
  EXPECT_LT(abs(et_m_per_s-8.594743e-08), 1.0e-08);
  ASSERT_TRUE(true);
}
