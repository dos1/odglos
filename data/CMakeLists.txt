if (EMSCRIPTEN)
	set(DATA_EXCLUDE_REGEX "\\./animations/|\\./sprites/|\\./pergola/|\\./byk/|\\./naparstki/|\\./armata/|\\./lawka_w_parku/")
endif(EMSCRIPTEN)

include(libsuperderpy-data)

if (EMSCRIPTEN)
	install(DIRECTORY byk DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data00")
	install(DIRECTORY sprites/byk DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data00/sprites")
	install(DIRECTORY sprites/dmuchawa DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data00/sprites")
	install(DIRECTORY sprites/generator DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data00/sprites")

	set(ANIMATIONS
		kostki_animacja01_tworzy_sie_kostka.awebp
		kostki_animacja02_cwierc_obrotu_zapetlic.awebp
		kostki_animacja03_waz.awebp
		wedrowka_rodzinki_po_trawce.awebp
		rodzinka_jak_wiewiorki.awebp
		sowka_wchodzi_na_drzewo.awebp
		sowka_pokazuje_mordke_i_wraca.awebp
		buty_drewniane.awebp
		regal_animacja_sam.awebp
		generator_animacja_wstepna.awebp
		sucha_trawa_aksamitki_samochod_stary.awebp
		sowka_konfrontacja_z_rzezba.awebp
		wrzosy_kuzyn_i_sowka2.awebp
		samochod_kominek.awebp
		swiecznik_hover_ewentualnie.awebp
		DSCF2305.awebp
		DSCF2449.awebp
		DSCF2311.awebp
		DSCF2320.awebp
		swiecznik3_TAK.awebp
	)

	foreach (FILE IN ITEMS ${ANIMATIONS})
		install(FILES animations/${FILE} DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data00/animations")
	endforeach()

	# ----------------

	install(DIRECTORY lawka_w_parku DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data01")

	set(ANIMATIONS
		samochody_w_lesie.awebp
		aksamitki_samochod_sowka.awebp
		donice_02_samochod_duzy_jedzie_w_lewo.awebp
		donice_10_sowka_srednia_wjezdza_do_donicy_z_prawej.awebp
		donice_01_samochod_duzy_jedzie_w_prawo.awebp
		donice_08_mala_sowka_z_samochodem_wyjezdza_w_przod.awebp
		skrzypce2_animacja_przerywnikowa.awebp
		skrzypce2_dzwiek1.awebp
		skrzypce2_dzwiek2.awebp
		skrzypce2_dzwiek3.awebp
		skrzypce2_dzwiek4.awebp
		skrzypce2_dzwiek5.awebp
		skrzypce2_dzwiek6.awebp
		skrzypce2_animacja_koncowa.awebp
		gawron_i_drewniany_medrzec.awebp
		ciemna_trawa_samochod_sowka.awebp
		ciemna_trawa_waz.awebp
		wchodzenie_po_schodach_samochod_sowka.awebp
		schodzenie_ze_schodow_waz.awebp
	)

	foreach (FILE IN ITEMS ${ANIMATIONS})
		install(FILES animations/${FILE} DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data01/animations")
	endforeach()

	# -------------------

	set(ANIMATIONS
		animacja_silacz1.awebp
		donice_14_samochod_nadjezdza_z_prawej_i_wjezdza_do_donicy_z_lewej.awebp
		donice_18_samochod_duzy_wyjezdza_w_prawo.awebp
		donice_06_samochod_maly_z_sowka_wyjezdza_w_przod.awebp
		donice_03_samochod_duzy_wjezdza_do_donicy_z_lewej.awebp
		donice_19_samochod_maly_z_sowka_wjezdza_z_prawej.awebp
		donice_17_samochod_duzy_wyjezdza_w_lewo.awebp
		donice_05_samochod_maly_bez_sowki_wyjezdza_w_przod.awebp
		donice_20_samochod_maly_bez_sowki_wjezdza_z_prawej.awebp
		donice_04_samochod_maly_z_sowka_wyjezdza_w_prawo.awebp
		donice_21_sowka_rudnik_wychodzi_w_prawo.awebp
		donice_12_waz_idzie_w_prawo_i_wchodzi_do_prawej_donicy.awebp
		silacz2_maly_samochod_z_sowka_opuszcz_cien_rozny.awebp
		silacz3_maly_samochod_sam.awebp
		lira_korbowa.awebp
		male_dziwne_cos.awebp
		turkusowe_cos.awebp
		rzezby_w_lazience_2_wyciszenie_sznureczka.awebp
		sowka_i_rzezby_01_sowka_przejezdza.awebp
		sowka_i_rzezby_02a_zakochani.awebp
		sowka_i_rzezby_02b_muzykanci.awebp
		031_donice_dom1.awebp
		donice_16_samochod_kartonowy_duzy_wjezdza_z_prawej.awebp
		donice_22_sowka_srednia_whodzi_do_duzej_donicy_z_lewej.awebp
		donice_15_maly_samochodzik_kartonowy_wyjezdza.awebp
		donice_23_sowka_mala_wychodzi_w_przod.awebp
		donice_24_sowka_mala_wchodzi_z_prawej.awebp
		donice_25_sowka_srednia_wychodzi_z_lewej_donicy.awebp
		donice_26_sowka_srednia_wchodzi_do_prawej_donicy.awebp
		donice_27_sowka_duza_wychodzi_z_lewej_donicy.awebp
		donice_11_waz_buszuje_w_prawo_w_lewo_i_wchodzi_z_lewej.awebp
		donica_w_hortensjach_06_waz.awebp
		aksamitki_waz.awebp
		waz_zmienia_sie_w_kostke.awebp
	)

	foreach (FILE IN ITEMS ${ANIMATIONS})
		install(FILES animations/${FILE} DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data02/animations")
	endforeach()

	# -------------------

	install(DIRECTORY pergola DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data03")

	set(ANIMATIONS
		sowka_wchodzi_do_miski_ciemniejsze.awebp
		duza_sowka_na_drewnianym_kole.awebp
		animacja_poczatkowa.awebp
		pergola_animacja_koncowa2.awebp
		pergola_animacja_koncowa6.awebp
		ul_duzy_pusty_mozna_dac_tez_sama_pierwsza_klatke.awebp
		ul_duzy_dol.awebp
		ul_duzy_lewo.awebp
		ul_duzy_gora.awebp
		ul_duzy_animacja_koncowa_samochod.awebp
	)

	foreach (FILE IN ITEMS ${ANIMATIONS})
		install(FILES animations/${FILE} DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data03/animations")
	endforeach()

	# -------------------

	install(DIRECTORY sprites/pudelka DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data04/sprites")

	set(ANIMATIONS
		pudelko_w_ogrodzie.awebp
		portal_ze_stolika_bialego.awebp
		siatka_na_drzewie_myszka.awebp
		drzewko_kolorowe1_maskotki_podwojne_moze_lepsze_TAK.awebp
	)

	foreach (FILE IN ITEMS ${ANIMATIONS})
		install(FILES animations/${FILE} DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data04/animations")
	endforeach()

	# -------------------

	install(DIRECTORY naparstki DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data05")

	set(ANIMATIONS
		naparstki_00_poczatek.awebp
		naparstki_01_kapelusz_jaszczurka_wersja1.awebp
		naparstki_01_kapelusz_jaszczurka_wersja2.awebp
		naparstki_01_kapelusz_jaszczurka_wersja3_najlepsza.awebp
		naparstki_02_jaszczurka_kredki.awebp
		naparstki_03_paski_okno.awebp
		naparstki_04_dzwoneczek_kwiatki.awebp
		naparstki_05_kwiatki_czarno_biale.awebp
		naparstki_06_czarno_bialy_bambus.awebp
	)

	foreach (FILE IN ITEMS ${ANIMATIONS})
		install(FILES animations/${FILE} DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data05/animations")
	endforeach()

	# -------------------

	set(ANIMATIONS
		naparstki_07_panda_babuszka_wersja_krotsza2_lepsza.awebp
		naparstki_08_babuszka_zlote.awebp
		naparstki_09_zloty_statki.awebp
		naparstki_10_latarnia_zbija_wszystko.awebp
		naparstki_10a_latarnia_pusta_sowka_piana_czesc1.awebp
		naparstki_10a_latarnia_pusta_sowka_piana_czesc2.awebp
		naparstki_10a_latarnia_pusta_sowka_piana_czesc3.awebp
		naparstki_10b_KONCOWKA_chodaki_owce.awebp
		naparstki_PUSTE_02_jaszczurka.awebp
	)

	foreach (FILE IN ITEMS ${ANIMATIONS})
		install(FILES animations/${FILE} DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data06/animations")
	endforeach()

	# -------------------

	install(DIRECTORY naparstki DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data07")

	set(ANIMATIONS
		naparstki_PUSTE_03_paski.awebp
		naparstki_PUSTE_04_dzwoneczek.awebp
		naparstki_PUSTE_05_kwiatek.awebp
		naparstki_PUSTE_06_czarno_bialy.awebp
		naparstki_PUSTE_07_panda.awebp
		naparstki_PUSTE_08_babuszka.awebp
		naparstki_PUSTE_09_zloty.awebp
		naparstki_PUSTE_10_latarnia.awebp
		naparstki_PUSTE_11_niebieskie_cos.awebp
		naparstki_PUSTE_12_Krakow.awebp
		naparstki_PUSTE_13_Mallorca.awebp
		naparstki_PUSTE_14_sztuka_wspolczesna.awebp
		naparstki_PUSTE_15_Toscania.awebp
		naparstki_PUSTE_16_maluszek.awebp
		naparstki_PUSTE_17_Praha.awebp
		pudelko_wypluwa_szczypczyki_smok_bez_dyn_TAK.awebp
	)

	foreach (FILE IN ITEMS ${ANIMATIONS})
		install(FILES animations/${FILE} DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data07/animations")
	endforeach()

	# -------------------

	install(DIRECTORY sprites/makieta DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data08/sprites")
	install(DIRECTORY armata DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data08")

	set(ANIMATIONS
		01statki_szyszki_tasmy_animacja1.awebp
		02statki_szyszki_tasmy_animacja2.awebp
		03statki_szyszki_tasmy_animacja3.awebp
		04statki_szyszki_tasmy_gra_dzwiek1.awebp
		04statki_szyszki_tasmy_gra_dzwiek2.awebp
		04statki_szyszki_tasmy_gra_dzwiek3.awebp
		05statki_szyszki_tasmy_animacja4.awebp
		06statki_szyszki_tasmy_animacja5.awebp
		magnetofon2_bez_myszek.awebp
		duch_portalu_animacja2_zlozona_TAK.awebp
		podniebny_generator_z_kosmosem.awebp
	)

	foreach (FILE IN ITEMS ${ANIMATIONS})
		install(FILES animations/${FILE} DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data08/animations")
	endforeach()

	# -------------------

	set(ANIMATIONS
		makieta_w_kosmosie_bez_tla.awebp
		makieta_pusta.awebp
		krzeslo_w_lesie_czesc1.awebp
		krzeslo_w_lesie_czesc2.awebp
		sowka1_wchodzi_na_stol_z_bliska_pojawia_sie_TAK.awebp
		sowka2_zaluzje_pojawia_sie2_TAK.awebp
		sowki_zamieniaja_sie_krzeslami_po_dwa_i_nie_znikaja_TAK.awebp
		sowki_zamieniaja_sie_krzeslami_po_dwa_freeze.awebp
		sowka1_wlacza_konsole_z_daleka2.awebp
		sowka1_wlacza_konsole_z_bliska_lewa_konsola.awebp
		sowka1_wlacza_konsole_z_bliska_srodkowa_konsola.awebp
		sowka2_klika_konsole_prawa.awebp
		sowka2_klika_konsole_lewa.awebp
		sowka1_wchodzi_na_stol_z_bliska_nie_znika_TAK.awebp
		altanka.awebp
		altanka_samochod.awebp
		zamiana_myszki_w_bramie.awebp
		buzia_01_bez_niczego.awebp
		myszkowanie_w_wiklinie_puste.awebp
		myszkowanie_w_wiklinie2.awebp
	)

	foreach (FILE IN ITEMS ${ANIMATIONS})
		install(FILES animations/${FILE} DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data09/animations")
	endforeach()


	# -------------------

	set(ANIMATIONS
		wiklinowe_kolo1_samochod.awebp
		wiklinowe_kolo3_myszka.awebp
		wiklinowy_cyrk_po_dwa_bez_myszki.awebp
		wiklinowy_cyrk_sama_myszka.awebp
		drzwi_zamykaja_sie_same.awebp
		okna_sie_otwieraja_z_sowka2.awebp
		sowka2_zaluzje_nie_znika_TAK.awebp
		sowka1_zaluzje.awebp
		drzwi_z_zewnatrz.awebp
		animacja_koncowa.awebp
		animacje_koncowe_rodzinki.awebp
		donice_13_tasma.awebp
	)

	foreach (FILE IN ITEMS ${ANIMATIONS})
		install(FILES animations/${FILE} DESTINATION "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/data10/animations")
	endforeach()

	# -----

	if(IMGTOWEBP)
		add_custom_target(${LIBSUPERDERPY_GAMENAME}_assets_img_to_webp
			DEPENDS ${ASSET_PIPELINE_DEPEND}
			COMMAND ${CMAKE_COMMAND} -DQUALITY="${IMGTOWEBP_QUALITY}" -DRESIZE="${IMGTOWEBP_SCALE}%" -DPARAMS="${IMGTOWEBP_PARAMS}" -DCACHE="${CMAKE_SOURCE_DIR}/.assetcache" -DLOSSLESS="${IMGTOWEBP_LOSSLESS}" -DDATADIR="${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets" -P ${LIBSUPERDERPY_DIR}/cmake/ImgToWebp.cmake
			USES_TERMINAL)
	else(IMGTOWEBP)
		add_custom_target(${LIBSUPERDERPY_GAMENAME}_assets_img_to_webp
			DEPENDS ${ASSET_PIPELINE_DEPEND})
	endif(IMGTOWEBP)

	MACRO(add_asset_pack_target PACK_NAME)
		add_custom_target(${LIBSUPERDERPY_GAMENAME}_assets-${PACK_NAME}
			DEPENDS ${LIBSUPERDERPY_GAMENAME}_install ${LIBSUPERDERPY_GAMENAME}_flac_to_lossy ${LIBSUPERDERPY_GAMENAME}_img_to_webp ${LIBSUPERDERPY_GAMENAME}_assets_img_to_webp
			WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${LIBSUPERDERPY_GAMENAME}/assets/${PACK_NAME}"
			COMMAND ${CMAKE_COMMAND} -E tar cfv ${CMAKE_INSTALL_PREFIX}/${LIBSUPERDERPY_GAMENAME}/${PACK_NAME}.zip --format=zip "*")
		add_dependencies(${LIBSUPERDERPY_GAMENAME}_js ${LIBSUPERDERPY_GAMENAME}_assets-${PACK_NAME})
	ENDMACRO()

	add_asset_pack_target(data00)
	add_asset_pack_target(data01)
	add_asset_pack_target(data02)
	add_asset_pack_target(data03)
	add_asset_pack_target(data04)
	add_asset_pack_target(data05)
	add_asset_pack_target(data06)
	add_asset_pack_target(data07)
	add_asset_pack_target(data08)
	add_asset_pack_target(data09)
	add_asset_pack_target(data10)

endif(EMSCRIPTEN)
