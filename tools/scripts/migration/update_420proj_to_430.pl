use strict;
use Data::Dumper;

my ($path) = @ARGV;


print "process MODULE driver/board/component/wifi/mt5931\n";
updateGCC("include \$(SOURCE_DIR)/driver/board/component/wifi/mt5931/module.mk", "driver/board/component/wifi/mt5931", "");

print "process MODULE driver/board/mt25x3_hdk/wifi/mt5931\n";
updateGCC("include \$(SOURCE_DIR)/driver/board/mt25x3_hdk/wifi/mt5931/module.mk", "driver/board/mt25x3_hdk/wifi/mt5931", "");

print "process MODULE middleware/MTK/wifi_service\n";

my $file_set = `grep -rl 'include \$(SOURCE_DIR)/driver/board/mt76x7_hdk/wifi/module.mk' $path`;
my @files = split(/\n/, $file_set);

foreach my $f (@files)
{

        print "File $f\n";
        print `sed -i 's/driver\\/board\\/mt76x7_hdk\\/wifi\\/module.mk/middleware\\/MTK\\/wifi_service\\/combo\\/module.mk\\ninclude \$(SOURCE_DIR)\\/middleware\\/MTK\\/connsys\\/module.mk/g' $f`;
}

#common
updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\get_profile_string.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\get_profile_string.c");

updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\wifi_os_api.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\wifi_os_api.c");


updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\misc.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\misc.c");
updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\os_util.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\wifi_os_ext\\\\os_util.c");
updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\os_utils.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\wifi_os_ext\\\\os_utils.c");
updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\wifi_channel.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\wifi_channel.c");
updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\wifi_cli.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\wifi_cli.c");
updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\wifi_default_config.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\init\\\\wifi_default_config.c");


updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\wifi_inband_cli.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\wifi_inband_cli.c");

updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\wifi_init.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\init\\\\wifi_init.c");

updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\wifi_profile.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\wifi_profile.c");

updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\wifi_wps.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\wifi_wps.c");

updateKEIL_IAR("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\src\\\\wifi_private_cli.c",
                "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\src\\\\wifi_private_cli.c");

updateKEIL_IAR("driver\\\\chip\\\\mt7687\\\\src\\\\sdio_gen3\\\\connsys_bus.c",
                "middleware\\\\MTK\\\\connsys\\\\src\\\\connsys_bus.c");

updateKEIL_IAR("driver\\\\chip\\\\mt7687\\\\src\\\\sdio_gen3\\\\connsys_driver.c",
                "middleware\\\\MTK\\\\connsys\\\\src\\\\connsys_driver.c");

updateKEIL_IAR("driver\\\\chip\\\\mt7687\\\\src\\\\sdio_gen3\\\\connsys_util.c",
                "middleware\\\\MTK\\\\connsys\\\\src\\\\connsys_util.c");


# IAR part
replaceIARInc("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\inc<\\/state>",
               "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\inc<\\/state>");

appendIARInc("<state>\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\driver\\\\board\\\\mt76x7_hdk\\\\external_flash\\\\inc<\\/state>",
               "<state>\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\middleware\\\\MTK\\\\connsys\\\\inc<\\/state>");

updateKEIL_IAR(".\\\\driver\\\\board\\\\mt76x7_hdk\\\\lib\\\\libwifi_CM4_IAR.a",
                ".\\\\prebuilt\\\\middleware\\\\MTK\\\\wifi_service\\\\combo\\\\lib\\\\wifi_supp\\\\libwifi_CM4_IAR.a");

#KEIL part
replaceKEILInc("driver\\\\board\\\\mt76x7_hdk\\\\wifi\\\\inc;",
               "middleware\\\\MTK\\\\wifi_service\\\\combo\\\\inc");

appendKEILInc(";..\\\\..\\\\..\\\\..\\\\..\\\\driver\\\\board\\\\mt76x7_hdk\\\\external_flash\\\\inc",
                ";..\\\\..\\\\..\\\\..\\\\..\\\\middleware\\\\MTK\\\\connsys\\\\inc");

updateKEIL_IAR(".\\\\driver\\\\board\\\\mt76x7_hdk\\\\lib\\\\libwifi_CM4_Keil.lib",
                ".\\\\prebuilt\\\\middleware\\\\MTK\\\\wifi_service\\\\combo\\\\lib\\\\wifi_supp\\\\libwifi_CM4_Keil.lib");


print "process MODULE driver/chip/mtxxxx\n";

updateKEIL_IAR("..\\\\..\\\\..\\\\..\\\\..\\\\driver\\\\chip\\\\mt2523\\\\lib\\\\libcryptos_CM4_Keil.lib","..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\driver\\\\chip\\\\mt2523\\\\lib\\\\libcryptos_CM4_Keil.lib");
updateKEIL_IAR("..\\\\..\\\\..\\\\..\\\\..\\\\driver\\\\chip\\\\mt2523\\\\lib\\\\libhal_core_CM4_Keil.lib","..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\driver\\\\chip\\\\mt2523\\\\lib\\\\libhal_core_CM4_Keil.lib");
updateKEIL_IAR("..\\\\..\\\\..\\\\..\\\\..\\\\driver\\\\chip\\\\mt2523\\\\lib\\\\libhal_protected_CM4_Keil.lib","..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\driver\\\\chip\\\\mt2523\\\\lib\\\\libhal_protected_CM4_Keil.lib");

updateKEIL_IAR("..\\\\..\\\\..\\\\..\\\\..\\\\driver\\\\chip\\\\mt7687\\\\lib\\\\libhal_core_CM4_Keil.lib","..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\driver\\\\chip\\\\mt7687\\\\lib\\\\libhal_core_CM4_Keil.lib");
updateKEIL_IAR("..\\\\..\\\\..\\\\..\\\\..\\\\driver\\\\chip\\\\mt7687\\\\lib\\\\libhal_protected_CM4_Keil.lib","..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\driver\\\\chip\\\\mt7687\\\\lib\\\\libhal_protected_CM4_Keil.lib");


updateKEIL_IAR("\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\driver\\\\chip\\\\mt2523\\\\lib\\\\libhal_core_CM4_IAR.a", "\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\driver\\\\chip\\\\mt2523\\\\lib\\\\libhal_core_CM4_IAR.a");
updateKEIL_IAR("\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\driver\\\\chip\\\\mt2523\\\\lib\\\\libhal_protected_CM4_IAR.a", "\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\driver\\\\chip\\\\mt2523\\\\lib\\\\libhal_protected_CM4_IAR.a");

updateKEIL_IAR("\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\driver\\\\chip\\\\mt7687\\\\lib\\\\libhal_core_CM4_IAR.a", "\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\driver\\\\chip\\\\mt7687\\\\lib\\\\libhal_core_CM4_IAR.a");
updateKEIL_IAR("\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\driver\\\\chip\\\\mt7687\\\\lib\\\\libhal_protected_CM4_IAR.a", "\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\driver\\\\chip\\\\mt7687\\\\lib\\\\libhal_protected_CM4_IAR.a");


print "process MODULE middleware/MTK/ble_ancs\n";
updateGCC("include \$(SOURCE_DIR)/middleware/MTK/ble_ancs/module.mk", "middleware/MTK/ble_ancs", "");
appendIARInc("<state>\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\middleware\\\\MTK\\\\ble_ancs\\\\inc<\\/state>", 
		"<state>\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\middleware\\\\MTK\\\\ble_ancs\\\\inc<\\/state>");
appendKEILInc("..\\\\..\\\\..\\\\..\\\\..\\\\middleware\\\\MTK\\\\ble_ancs\\\\inc;",
		"..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\middleware\\\\MTK\\\\ble_ancs\\\\inc");
updateKEIL_IAR("..\\\\..\\\\..\\\\..\\\\..\\\\middleware\\\\MTK\\\\ble_ancs\\\\lib\\\\libbleancs_Keil.lib", "..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\middleware\\\\MTK\\\\ble_ancs\\\\lib\\\\libbleancs_Keil.lib");
updateKEIL_IAR("..\\\\middleware\\\\MTK\\\\ble_ancs\\\\lib\\\\libbleancs_IAR.a", "..\\\\prebuilt\\\\middleware\\\\MTK\\\\ble_ancs\\\\lib\\\\libbleancs_IAR.a");

print "process MODULE middleware/MTK/bt_notify\n";
updateKEIL_IAR("..\\\\..\\\\..\\\\..\\\\..\\\\middleware\\\\MTK\\\\bt_notify\\\\lib\\\\libbtnotify_Keil.lib", "..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\middleware\\\\MTK\\\\bt_notify\\\\lib\\\\libbtnotify_Keil.lib");
updateKEIL_IAR("..\\\\middleware\\\\MTK\\\\bt_notify\\\\lib\\\\libbtnotify_IAR.a", "..\\\\prebuilt\\\\middleware\\\\MTK\\\\bt_notify\\\\lib\\\\libbtnotify_IAR.a");

print "process MODULE middleware/MTK/homekit\n";
updateGCC("include \$(SOURCE_DIR)/middleware/MTK/homekit/module.mk", "middleware/MTK/homekit", "");

print "process MODULE middleware/MTK/minicli\n";
updateKEIL_IAR(".\\\\middleware\\\\MTK\\\\minicli\\\\lib\\\\libminicli_CM4_Keil.lib", ".\\\\prebuilt\\\\middleware\\\\MTK\\\\minicli\\\\lib\\\\libminicli_CM4_Keil.lib");
updateKEIL_IAR(".\\\\middleware\\\\MTK\\\\minicli\\\\lib\\\\libminicli_CM4_IAR.a", ".\\\\prebuilt\\\\middleware\\\\MTK\\\\minicli\\\\lib\\\\libminicli_CM4_IAR.a");

print "process MODULE middleware/MTK/minisupp\n";
updateGCC("include \$(SOURCE_DIR)/middleware/MTK/minisupp/module.mk", "middleware/MTK/minisupp", "");

updateKEIL_IAR(".\\\\middleware\\\\MTK\\\\minisupp\\\\lib\\\\libminisupp_CM4_Keil.lib", ".\\\\prebuilt\\\\middleware\\\\MTK\\\\minisupp\\\\lib\\\\libminisupp_CM4_Keil.lib");
updateKEIL_IAR(".\\\\middleware\\\\MTK\\\\minisupp\\\\lib\\\\libminisupp_CM4_IAR.a", ".\\\\prebuilt\\\\middleware\\\\MTK\\\\minisupp\\\\lib\\\\libminisupp_CM4_IAR.a");

updateKEIL_IAR(".\\\\middleware\\\\MTK\\\\minisupp\\\\lib\\\\libminisupp_wps_CM4_Keil.lib", ".\\\\prebuilt\\\\middleware\\\\MTK\\\\minisupp\\\\lib\\\\libminisupp_wps_CM4_Keil.lib");
updateKEIL_IAR(".\\\\middleware\\\\MTK\\\\minisupp\\\\lib\\\\libminisupp_wps_CM4_IAR.a", ".\\\\prebuilt\\\\middleware\\\\MTK\\\\minisupp\\\\lib\\\\libminisupp_wps_CM4_IAR.a");


replaceIARInc("middleware\\\\MTK\\\\minisupp\\\\inc<\\/state>",
               "prebuilt\\\\middleware\\\\MTK\\\\minisupp\\\\inc<\\/state>");

replaceKEILInc("middleware\\\\MTK\\\\minisupp\\\\inc;",
               "prebuilt\\\\middleware\\\\MTK\\\\minisupp\\\\inc");


print "process MODULE middleware/MTK/minorsupc\n";
updateGCC("include \$(SOURCE_DIR)/middleware/MTK/minorsupc/module.mk", "middleware/MTK/minorsupc", "");


print "process MODULE middleware/MTK/nvdm\n";
appendIARInc("<state>\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\middleware\\\\MTK\\\\nvdm\\\\inc_core<\\/state>",
               "<state>\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\middleware\\\\MTK\\\\nvdm\\\\inc<\\/state>");

appendKEILInc("..\\\\..\\\\..\\\\..\\\\..\\\\middleware\\\\MTK\\\\nvdm\\\\inc_core;",
               "..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\middleware\\\\MTK\\\\nvdm\\\\inc");


updateKEIL_IAR(".\\\\middleware\\\\MTK\\\\nvdm\\\\lib\\\\libnvdm_CM4_KEIL.lib", ".\\\\prebuilt\\\\middleware\\\\MTK\\\\nvdm\\\\lib\\\\libnvdm_CM4_KEIL.lib");
updateKEIL_IAR(".\\\\middleware\\\\MTK\\\\nvdm\\\\lib\\\\libnvdm_CM4_IAR.a", ".\\\\prebuilt\\\\middleware\\\\MTK\\\\nvdm\\\\lib\\\\libnvdm_CM4_IAR.a");

print "process MODULE middleware/MTK/slp\n";
updateGCC("include \$(SOURCE_DIR)/middleware/MTK/slp/module.mk", "middleware/MTK/slp", "");

print "process MODULE middleware/MTK/smtcn\n";
updateKEIL_IAR(".\\\\middleware\\\\MTK\\\\smtcn\\\\lib\\\\libsmtcn_CM4_Keil.lib", ".\\\\prebuilt\\\\middleware\\\\MTK\\\\smtcn\\\\lib\\\\libsmtcn_CM4_Keil.lib");
updateKEIL_IAR(".\\\\middleware\\\\MTK\\\\smtcn\\\\lib\\\\libsmtcn_CM4_IAR.a", ".\\\\prebuilt\\\\middleware\\\\MTK\\\\smtcn\\\\lib\\\\libsmtcn_CM4_IAR.a");


print "process MODULE middleware/MTK/sensor_subsys/fusion_algo/\n";

updateKEIL_IAR("\\\\middleware\\\\MTK\\\\sensor_subsys\\\\fusion_algo\\\\",
                "\\\\prebuilt\\\\middleware\\\\MTK\\\\fusion_algo\\\\");

print "process MODULE driver/board/component/audio\n";
updateGCC2("include \$(SOURCE_DIR)/driver/board/component/audio/lib/arm_cm4/module.mk", "driver/board/component/audio", "");
updateKEIL_IAR("driver\\\\board\\\\component\\\\audio\\\\lib\\\\arm_cm4\\\\libblisrc_CM4_IAR.a", "prebuilt\\\\driver\\\\board\\\\component\\\\audio\\\\lib\\\\arm_cm4\\\\libblisrc_CM4_IAR.a");
updateKEIL_IAR("driver\\\\board\\\\component\\\\audio\\\\lib\\\\arm_cm4\\\\libblisrc_CM4_Keil.lib", "prebuilt\\\\driver\\\\board\\\\component\\\\audio\\\\lib\\\\arm_cm4\\\\libblisrc_CM4_Keil.lib");
replaceIARInc("driver\\\\board\\\\component\\\\audio\\\\inc<\\/state>",
               "prebuilt\\\\driver\\\\board\\\\component\\\\audio\\\\inc<\\/state>");
replaceKEILInc("driver\\\\board\\\\component\\\\audio\\\\inc;",
               "prebuilt\\\\driver\\\\board\\\\component\\\\audio\\\\inc");

print "process MODULE driver/board/component/bt_codec\n";
updateGCC2("include \$(SOURCE_DIR)/driver/board/component/bt_codec/lib/arm_cm4/module.mk", "driver/board/component/bt_codec", "");

replaceIARInc("driver\\\\board\\\\component\\\\bt_codec\\\\inc<\\/state>",
               "prebuilt\\\\driver\\\\board\\\\component\\\\bt_codec\\\\inc<\\/state>");
replaceKEILInc("driver\\\\board\\\\component\\\\bt_codec\\\\inc;",
               "prebuilt\\\\driver\\\\board\\\\component\\\\bt_codec\\\\inc");

updateKEIL_IAR("driver\\\\board\\\\component\\\\bt_codec\\\\lib\\\\arm_cm4\\\\libcvsd_codec_CM4_IAR.a", "prebuilt\\\\driver\\\\board\\\\component\\\\bt_codec\\\\lib\\\\arm_cm4\\\\libcvsd_codec_CM4_IAR.a");
updateKEIL_IAR("driver\\\\board\\\\component\\\\bt_codec\\\\lib\\\\arm_cm4\\\\libcvsd_codec_CM4_Keil.lib", "prebuilt\\\\driver\\\\board\\\\component\\\\bt_codec\\\\lib\\\\arm_cm4\\\\libcvsd_codec_CM4_Keil.lib");
updateKEIL_IAR("driver\\\\board\\\\component\\\\bt_codec\\\\lib\\\\arm_cm4\\\\libmsbc_codec_CM4_IAR.a", "prebuilt\\\\driver\\\\board\\\\component\\\\bt_codec\\\\lib\\\\arm_cm4\\\\libmsbc_codec_CM4_IAR.a");
updateKEIL_IAR("driver\\\\board\\\\component\\\\bt_codec\\\\lib\\\\arm_cm4\\\\libmsbc_codec_CM4_Keil.lib", "prebuilt\\\\driver\\\\board\\\\component\\\\bt_codec\\\\lib\\\\arm_cm4\\\\libmsbc_codec_CM4_Keil.lib");
updateKEIL_IAR("driver\\\\board\\\\component\\\\bt_codec\\\\lib\\\\arm_cm4\\\\libplc_CM4_IAR.a", "prebuilt\\\\driver\\\\board\\\\component\\\\bt_codec\\\\lib\\\\arm_cm4\\\\libplc_CM4_IAR.a");
updateKEIL_IAR("driver\\\\board\\\\component\\\\bt_codec\\\\lib\\\\arm_cm4\\\\libplc_CM4_Keil.lib", "prebuilt\\\\driver\\\\board\\\\component\\\\bt_codec\\\\lib\\\\arm_cm4\\\\libplc_CM4_Keil.lib");

print "process MODULE mp3 codec\n";
updateKEIL_IAR("middleware\\\\MTK\\\\audio\\\\mp3_codec\\\\lib\\\\arm_cm4\\\\lib_mp3dec_CM4_Keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\audio\\\\mp3_codec\\\\lib\\\\arm_cm4\\\\lib_mp3dec_CM4_Keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\audio\\\\mp3_codec\\\\lib\\\\arm_cm4\\\\lib_mp3dec_CM4_IAR.a", "prebuilt\\\\middleware\\\\MTK\\\\audio\\\\mp3_codec\\\\lib\\\\arm_cm4\\\\lib_mp3dec_CM4_IAR.a");

print "process MODULE amr codec\n";
updateKEIL_IAR("middleware\\\\MTK\\\\audio\\\\amr_codec\\\\lib\\\\arm_cm4\\\\libamr_CM4_Keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\audio\\\\amr_codec\\\\lib\\\\arm_cm4\\\\libamr_CM4_Keil.lib");

print "process MODULE fuelgauge\n";
updateKEIL_IAR("middleware\\\\MTK\\\\battery_management\\\\port\\\\mt2523\\\\lib\\\\libfgauge_IAR.a", "prebuilt\\\\middleware\\\\MTK\\\\fuelgauge\\\\lib\\\\libfgauge_IAR.a");
updateKEIL_IAR("middleware\\\\MTK\\\\battery_management\\\\port\\\\mt2523\\\\lib\\\\libfgauge.lib", "prebuilt\\\\middleware\\\\MTK\\\\fuelgauge\\\\lib\\\\libfgauge.lib");

replaceKEILInc("middleware\\\\MTK\\\\battery_management\\\\port\\\\mt2523\\\\lib\\\\fuelgauge\\\\inc;",
               "prebuilt\\\\middleware\\\\MTK\\\\fuelgauge\\\\inc");

replaceIARInc("middleware\\\\MTK\\\\battery_management\\\\port\\\\mt2523\\\\lib\\\\fuelgauge\\\\inc<\\/state>",
               "prebuilt\\\\middleware\\\\MTK\\\\fuelgauge\\\\inc<\\/state>");

print "process MODULE bluetooth\n";
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libble_CM4_keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libble_CM4_keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libble_CM4_keil_release.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libble_CM4_keil_release.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_CM4_keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_CM4_keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_CM4_keil_release.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_CM4_keil_release.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_a2dp_CM4_keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_a2dp_CM4_keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_a2dp_CM4_keil_release.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_a2dp_CM4_keil_release.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libble_multi_adv_CM4_keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libble_multi_adv_CM4_keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_CM4_keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_CM4_keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_CM4_keil_release.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_CM4_keil_release.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_eh_CM4_keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_eh_CM4_keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_eh_CM4_keil_release.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_eh_CM4_keil_release.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_hfp_CM4_keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_hfp_CM4_keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_hfp_CM4_keil_release.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_hfp_CM4_keil_release.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_pbapc_CM4_keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_pbapc_CM4_keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_pbapc_CM4_keil_release.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_pbapc_CM4_keil_release.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_spp_CM4_keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_spp_CM4_keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_spp_CM4_keil_release.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_spp_CM4_keil_release.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_2523_CM4_keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_2523_CM4_keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_7697_CM4_keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_7697_CM4_keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_aws_CM4_keil.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_aws_CM4_keil.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_aws_CM4_keil_release.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_aws_CM4_keil_release.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_2523_CM4_keil_release.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_2523_CM4_keil_release.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_7697_CM4_keil_release.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_7697_CM4_keil_release.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_2523_CM4_keil_tx.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_2523_CM4_keil_tx.lib");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_2523_CM4_keil_release_tx.lib", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_2523_CM4_keil_release_tx.lib");

updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libble_iar.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libble_iar.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libble_iar_release.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libble_iar_release.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_a2dp_iar.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_a2dp_iar.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_a2dp_iar_release.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_a2dp_iar_release.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libble_multi_adv_iar.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libble_multi_adv_iar.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_enhance_iar.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_enhance_iar.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_enhance_iar_release.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_enhance_iar_release.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_iar.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_iar.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_iar_release.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_avrcp_iar_release.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_driver_2523_iar.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_driver_2523_iar.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_driver_2523_iar_release.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_driver_2523_iar_release.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_hfp_iar.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_hfp_iar.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_hfp_iar_release.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_hfp_iar_release.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_iar.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_iar.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_iar_release.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_iar_release.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_pbapc_iar.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_pbapc_iar.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_pbapc_iar_release.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_pbapc_iar_release.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_spp_iar.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_spp_iar.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_spp_iar_release.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_spp_iar_release.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_7697_iar.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_7697_iar.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_7697_iar_release.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_7697_iar_release.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_aws_iar.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_aws_iar.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_aws_iar_release.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_aws_iar_release.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_driver_2523_iar_tx.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_driver_2523_iar_tx.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_driver_2523_iar_release_tx.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbt_driver_2523_iar_release_tx.a");
updateKEIL_IAR("middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_2523.a", "prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\lib\\\\libbtdriver_2523.a");

appendIARInc("<state>\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\middleware\\\\MTK\\\\bluetooth\\\\inc<\\/state>",
               "<state>\$PROJ_DIR\$\\\\..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\inc<\\/state>");
appendKEILInc("..\\\\..\\\\..\\\\..\\\\..\\\\middleware\\\\MTK\\\\bluetooth\\\\inc;",
               "..\\\\..\\\\..\\\\..\\\\..\\\\prebuilt\\\\middleware\\\\MTK\\\\bluetooth\\\\inc");

addBTInc();

# httpd module
print "Process dhcpd module\n";
my $file_set = `grep -rl 'third_party/dhcpd' $path`;
my @files = split(/\n/, $file_set);

foreach my $f (@files)
{

	print "File $f\n";
        print `sed -i 's/third_party\\/dhcpd/MTK\\/dhcpd/g' $f`;
}

my $file_set = `grep -rl 'third_party\\\\dhcpd' $path`;
my @files = split(/\n/, $file_set);

foreach my $f (@files)
{
	print "File $f\n";
        print `sed -i 's/third_party\\\\dhcpd/MTK\\\\dhcpd/g' $f`;
}

#kernel module
print "process kernel module\n";

#### IAR

my $keyword = "libkservice_CM4_MT2523_IAR.a";
my $rslt = `grep -lr '$keyword' $path`;
replace_kernel_IAR($keyword, $rslt);

my $keyword = "libkservice_CM4_MT7687_IAR.a";
my $rslt = `grep -lr '$keyword' $path`;
replace_kernel_IAR($keyword, $rslt);

sub replace_kernel_IAR
{
	my $keyword = shift;
	my $rslt = shift;


	my @lines = split(/\n/, $rslt);

	my $target = <<'LINE';
    <file>
      <name>$PROJ_DIR$\..\..\..\..\..\kernel\service\lib\$keyword</name>
    </file>
LINE

	my $replacement = <<'NEWLINE';
    <file>
      <name>$PROJ_DIR$\..\..\..\..\..\kernel\service\src\context_info_save.c</name>
    </file>
    <file>
      <name>$PROJ_DIR$\..\..\..\..\..\kernel\service\src\exception_handler.c</name>
    </file>
    <file>
      <name>$PROJ_DIR$\..\..\..\..\..\kernel\service\src\syslog.c</name>
    </file>
    <file>
      <name>$PROJ_DIR$\..\..\..\..\..\kernel\service\src\toi.c</name>
    </file>
NEWLINE

my $quote_target = quotemeta($target);
my $quote_new = quotemeta($replacement);

	foreach my $l (@lines)
	{

		print "open $l\n";

		open my $info, $l or die "Could not open $l: $!";

		open F,">log";

		my $prev="";
		my $skip=0;
		while( my $cur = <$info>)
		{
			if($skip > 1)
			{
				$skip--;
				next;
			}

			if($cur =~ /<name>kernel/)
			{
				print F $prev;
				$prev = $cur.$replacement;
				next;
			}

			if($cur =~ /$keyword/)
			{
				$prev = "";
				$skip = 2;
			}
			else
			{
				print F $prev;
				$prev = $cur;

			}
		
		}
		close $info;
		print F $prev;
		close(F);

		print `cp log $l`;
	}
}

### KEIL
my $keyword = "libkservice_CM4_MT2523_Keil.lib";
my $rslt = `grep -lr '$keyword' $path`;
replace_kernel_KEIL($keyword, $rslt);

my $keyword = "libkservice_CM4_MT7687_Keil.lib";
my $rslt = `grep -lr '$keyword' $path`;
replace_kernel_KEIL($keyword, $rslt);


sub replace_kernel_KEIL
{
        my $keyword = shift;
        my $rslt = shift;


        my @lines = split(/\n/, $rslt);


	my $target = <<'LINE';
    <file>
      <name>$PROJ_DIR$\..\..\..\..\..\kernel\service\lib\$keyword</name>
    </file>
LINE

	my $replacement = <<'NEWLINE';
            <File>
              <FileName>context_info_save.c</FileName>
              <FileType>1</FileType>
              <FilePath>..\..\..\..\..\kernel\service\src\context_info_save.c</FilePath>
            </File>
            <File>
              <FileName>exception_handler.c</FileName>
              <FileType>1</FileType>
              <FilePath>..\..\..\..\..\kernel\service\src\exception_handler.c</FilePath>
            </File>
            <File>
              <FileName>syslog.c</FileName>
              <FileType>1</FileType>
              <FilePath>..\..\..\..\..\kernel\service\src\syslog.c</FilePath>
            </File>
            <File>
              <FileName>toi.c</FileName>
              <FileType>1</FileType>
              <FilePath>..\..\..\..\..\kernel\service\src\toi.c</FilePath>
            </File>
NEWLINE

	my $quote_target = quotemeta($target);
	my $quote_new = quotemeta($replacement);

	foreach my $l (@lines)
	{
		if($l =~ /uvoptx/ || $l =~/no_rtos_initialize_system/)
		{
			next;
		}

                if($l =~ /2523_hdk/)
                {
                        if( $l =~/g2d_overlay_layer/ ||  $l =~/no_rtos_initialize_system/)
			{
				next;
			}
                }
		

		print "open $l\n";

		open my $info, $l or die "Could not open $l: $!";

		open F,">log";

		my $prev="";
		my $skip=0;
		while( my $cur = <$info>)
		{
			if($skip > 1)
			{
				$skip--;
				next;
			}

			if($cur =~ /$keyword/)
			{
				$prev = "";

				print F $replacement;
				$skip = 4;
			}
			else
			{
				print F $prev;
				$prev = $cur;

			}	
		
		}
		close $info;

		print F $prev;
		close(F);

		print `cp log $l`;
	}

}

#********************************************
# sub functions
#********************************************

sub addBTInc
{
	my $cmd = `grep -lr 'include \$(SOURCE_DIR)/middleware/MTK/bluetooth/module.mk' $path`;
	my @f1 = split(/\n/, $cmd);
	my %f1_hash;
	foreach my $f (@f1){$f1_hash{$f}=1;}

	my $cmd2 = `grep -lr 'CFLAGS += -I\$(SOURCE_DIR)/middleware/MTK/bluetooth/inc' $path`;
	my @f2 = split(/\n/, $cmd2);

	my $cmd3 = `grep -lr '#include \$(SOURCE_DIR)/middleware/MTK/bluetooth/module.mk' $path`;
	my @f3 = split(/\n/, $cmd3);
        my %f3_hash;
        foreach my $f (@f3){$f3_hash{$f}=1;}
	
	my %set;
	
	foreach my $f (@f2)
	{
		if($f1_hash{$f} ne "")
		{
			if($f3_hash{$f} eq 1)
			{
				$set{$f} = 1;
			}
		}
		else
		{
			$set{$f}=1;
		}
	}	

	foreach my $f (keys %set)
	{
		my $target = quotemeta("CFLAGS += -I\$(SOURCE_DIR)/middleware/MTK/bluetooth/inc");
		my $newline = "CFLAGS += -I\$(SOURCE_DIR)/middleware/MTK/bluetooth/inc\nCFLAGS += -I\$(SOURCE_DIR)/prebuilt/middleware/MTK/bluetooth/inc";
		my $content = `cat $f`;


		$content =~ s/$target/$newline/g;
		print "write to file $f\n";
		open F, ">$f";
		print F $content;
		close(F);
		
	}

}

sub replaceIARInc
{
        my $old = shift;
        my $new = shift;

        my $cmd = "grep -lr '$old' $path|xargs sed -i 's/$old/$new/g'";
        print $cmd."\n";
        my $rslt = `$cmd`;

}

sub replaceKEILInc
{
        my $old = shift;
        my $new = shift;

        my $cmd = "grep -lr '$old' $path|xargs sed -i 's/$old/$new;/g'";
        print $cmd."\n";
        my $rslt = `$cmd`;

}

sub appendIARInc
{
	my $old = shift;
	my $new = shift;

        my $cmd = "grep -lr '$old' $path|xargs sed -i 's/$old/$old\\n          $new/g'";
        print $cmd."\n";
        my $rslt = `$cmd`;
	
}

sub appendKEILInc
{
        my $old = shift;
        my $new = shift;

        my $cmd = "grep -lr '$old' $path|xargs sed -i 's/$old/$old$new;/g'";
        print $cmd."\n";
        my $rslt = `$cmd`;

}

sub updateGCC2
{
        my $target_line = shift;
        my $original_path = shift;
        my $new_path = shift;

        my $replace_str = "include \$(SOURCE_DIR)/prebuilt/<P2>/module.mk";

        my $rslt = `grep -lr '$target_line' $path`;
        my @files = split(/\n/, $rslt);

        foreach my $f (@files)
        {

                print "File ".$f."\n";
                my $content = `cat $f`;
                my @lines = split(/\n/, $content);

                open F, ">$f";

                my $quoted_target = quotemeta($target_line);
                foreach my $l (@lines)
                {
                        if($l =~ /$quoted_target/)
                        {
                                if ($new_path ne "")
                                {
                                        $replace_str =~ s/<P2>/$new_path/;
                                }
                                else
                                {
                                        $replace_str =~ s/<P2>/$original_path/;
                                }

                                print F $replace_str."\n";
                        }
                        else
                        {
                                print F $l."\n";
                        }
                }
        }

}

sub updateGCC
{
	my $target_line = shift;
	my $original_path = shift;
	my $new_path = shift;

	my $replace_str = <<'END_MSG';
ifneq ($(wildcard $(strip $(SOURCE_DIR))/<P1>/),)
include $(SOURCE_DIR)/<P1>/module.mk
else
include $(SOURCE_DIR)/prebuilt/<P2>/module.mk
endif
END_MSG

	my $rslt = `grep -lr '$target_line' $path`;
	my @files = split(/\n/, $rslt);

	foreach my $f (@files)
	{

		print "File ".$f."\n";
		my $content = `cat $f`;
		my @lines = split(/\n/, $content);

		open F, ">$f";

		my $quoted_target = quotemeta($target_line);
		foreach my $l (@lines)
		{
			if($l =~ /$quoted_target/)
			{
				$replace_str =~ s/<P1>/$original_path/g;

				if ($new_path ne "")
				{
					$replace_str =~ s/<P2>/$new_path/;
				}
				else
				{
					$replace_str =~ s/<P2>/$original_path/;
				}

				print F $replace_str."\n";
			}
			else
			{	
				print F $l."\n";	
			}
		}
	}
}


sub updateKEIL_IAR
{
	my $target_line=shift;
	my $new_line=shift;

	my $cmd = "grep -lr '$target_line' $path|xargs sed -i 's/$target_line/$new_line/g'";
	print $cmd."\n";
	my $rslt = `$cmd`;

}

