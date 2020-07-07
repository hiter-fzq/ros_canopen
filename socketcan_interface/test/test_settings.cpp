// Bring in my package's API, which is what I'm testing
#include <socketcan_interface/settings.h>
#include <socketcan_interface/socketcan.h>

// Bring in gtest
#include <gtest/gtest.h>

TEST(SettingTest, socketcan_masks)
{
  const can_err_mask_t fatal_errors = ( CAN_ERR_TX_TIMEOUT   /* TX timeout (by netdevice driver) */
                                      | CAN_ERR_BUSOFF       /* bus off */
                                      | CAN_ERR_BUSERROR     /* bus error (may flood!) */
                                      | CAN_ERR_RESTARTED    /* controller restarted */
                                        );
  const can_err_mask_t report_errors = ( CAN_ERR_LOSTARB      /* lost arbitration    / data[0]    */
                                       | CAN_ERR_CRTL         /* controller problems / data[1]    */
                                       | CAN_ERR_PROT         /* protocol violations / data[2..3] */
                                       | CAN_ERR_TRX          /* transceiver status  / data[4]    */
                                       | CAN_ERR_ACK          /* received no ACK on transmission */
                                       );
  can::SocketCANInterface sci;

  // defaults
  sci.init("None", false, can::NoSettings::create());
  EXPECT_EQ(sci.getErrorMask(), fatal_errors | report_errors);
  EXPECT_EQ(sci.getFatalErrorMask(), fatal_errors);

  // remove report-only flag
  auto m1 = can::SettingsMap::create();
  m1->set("error_mask/CAN_ERR_LOSTARB", false);
  sci.init("None", false, m1);
  EXPECT_EQ(sci.getErrorMask(), (fatal_errors | report_errors) & (~CAN_ERR_LOSTARB));
  EXPECT_EQ(sci.getFatalErrorMask(), fatal_errors);

  // cannot remove fatal flag from report only
  auto m2 = can::SettingsMap::create();
  m2->set("error_mask/CAN_ERR_TX_TIMEOUT", false);
  sci.init("None", false, m2);
  EXPECT_EQ(sci.getErrorMask(), (fatal_errors | report_errors));
  EXPECT_EQ(sci.getFatalErrorMask(), fatal_errors);

  // remove fatal flag
  auto m3 = can::SettingsMap::create();
  m3->set("fatal_error_mask/CAN_ERR_TX_TIMEOUT", false);
  sci.init("None", false, m3);
  EXPECT_EQ(sci.getErrorMask(), (fatal_errors | report_errors) & (~CAN_ERR_TX_TIMEOUT));
  EXPECT_EQ(sci.getFatalErrorMask(), fatal_errors & (~CAN_ERR_TX_TIMEOUT));

  // remove fatal and report flag
  auto m4 = can::SettingsMap::create();
  m4->set("fatal_error_mask/CAN_ERR_TX_TIMEOUT", false);
  m4->set("error_mask/CAN_ERR_LOSTARB", false);
  sci.init("None", false, m4);
  EXPECT_EQ(sci.getErrorMask(), (fatal_errors | report_errors) & ~(CAN_ERR_TX_TIMEOUT|CAN_ERR_LOSTARB));
  EXPECT_EQ(sci.getFatalErrorMask(), fatal_errors & (~CAN_ERR_TX_TIMEOUT));
}

// Run all the tests that were declared with TEST()
int main(int argc, char **argv){
testing::InitGoogleTest(&argc, argv);
return RUN_ALL_TESTS();
}
