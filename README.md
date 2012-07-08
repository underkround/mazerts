# MAZERTS

![mazeRTS](http://zemm.github.com/mazerts/mazerts.png)

[Kuvia kehityksen varrelta](http://ezbe.iio6s.info/kuvat/)

Imusta peli tästä

Jos peli valittaa dll:stä, hae Visual C++ 2008 Runtimes [täältä](http://www.microsoft.com/downloads/details.aspx?FamilyID=9b2da534-3e03-4391-8a4d-074b9f2bc1bf&displaylang=en). Jos tämän jälkeen jotain vielä puuttuu, on kyseessä luultavasti DirectX runtimet: [täältä](http://www.microsoft.com/downloads/details.aspx?FamilyId=2DA43D38-DB71-4C1B-BC6A-9B6652CD92A3&displaylang=en)

Resoluutio ja täyden ruudun moodi vaihdetaan `/data/config.ini` tiedostosta. Ikkunassa pelatessa ikkunan kokoa voi muuttaa kesken pelin, ruutu todennäköisesti menee hetkeksi mustaksi, mutta sitten peli jatkuu taas.

Erilaisia skenaarioita voi kokeilla vaihtamalla 'scenario file' asetusta samasta tiedostosta.
Vaihtoehtoiset skenaariot löytyvät `/data/scenarios/` -kansiosta. Oletuksena olevassa Pentagon -skenaariossa tekoälyvastustajia on peräti 4 mutta ne taistelevat jonkin verran toisiaankin vastaan. Jos tuntuu liian vaikealta skenaarioon on helppo kokeilla lisätä omia yksiköitä ;)

Vaikka peli onkin hyvin reaaliaikastategioiden perinteitä kunnioittava käyttöliittymältään niin tässä kuitenkin varulta pikaohjeet pelin pelaamiseen:

## Ohjaus
Peliä ohjataan hiirellä, pelialuetta "kiskotaan" pitämällä hiiren oikeaa nappia pohjassa ja liikuttamalla hiirtä samaan aikaan, kameraa voi kääntää pitämällä hiiren keskimmäistä nappia (rullaa) pohjassa ja hiirtä liikuttamalla. Ruutua voi liikuttaa myös nuolinapeista, tai klikkaamalla haluamaansa kohtaa minimapista (joka ilmestyy näkyviin vain kun pelaajalla on tutkarakennus ja riittävästi sähköä sen käyttöön). Hiiren rullaa pyörittelemällä kameraa voi zoomata lähemmäs ja kauemmas. Kontrolleja voi muokata `/data/controls.ini` tiedostosta.


## Rakentaminen
Ruudun vasemmassa laidassa on rakennuspainikkeet. Klikatessasi haluaamaasi talon kuvaa vasemmalla hiiren napilla, näkyy kursorisi alla maastossa talon pohjapinta-alaa kuvaava ruudukko (jos omaat riittävästi malmia kyseisen talon rakentamiseen). Talon voi asettaa kartalle, kun ruudukko on vihreänä vasemmalla napilla. Oikea hiiren nappi peruuttaa rakentamisen.


## Rakennusten ja yksiköiden ohjaus
Yksiköitä ja rakennuksia valitaan vasemmalla napilla, tyhjään kohtaan vasemmalla napilla klikkaamalla nykyinen valinta poistuu. Useamman yksikön voi valita "maalaamalla" hiiren vasen nappi pohjassa maastoa, jolloin näkyviin ilmestyy valintaruudukko. Myös vasenta CTRL-näppäintä pohjassa voi valita useampia yksiköitä yksi kerrallaan. Maalatessa valitaan vain yksiköitä (jos maalatulla alueella on sekä taloja että yksiköitä), ctrl pohjassa voi valita mitä haluaa.

Valittuja yksiköitä voi komentaa kartalla klikkaamalla haluttua kohdetta oikealla hiiren napilla joko maastosta tai minimapista (joka edelleen näkyy vain, kun pelaajalla on sekä tutkarakennus että riittävästi sähköä sen käyttöön). Jos kohde on maastoa, liikkuvat yksiköt määrättyyn paikkaan, vihollisen rakennusta tai yksikköä klikatessa ne hyökkäävät sen kimppuun. Useampia komentoja voi pistää jonoon pitämällä vasenta SHIFT-näppäintä pohjassa. Yksiköt hyökkäävät myös autonomisesti lähelle tulevien vihollisten kimppuun.

Osalla rakennuksista ja yksiköistä on omia nappejaan jotka tulevat näkyviin talon/yksikön ollessa valittuna, esimerkiksi yksiköiden liikkumisen voi pysäyttää stop napista.


## Yksiköiden rakentaminen
Tehtaan (War Factory) valitsemalla saat näkyviin listan yksiköistä joita tehtaassa voi rakentaa, näiden yksiköiden tuotantoa voi ketjuttaa useamman jonoon, huomaa kuitenkin että yksikön hinta laskutetaan jo jonoon laitettaessa. Jos tehdas tuhoutuu kun yksiköitä on vielä jonossa, on näihin käytetyt resurssit (malmi) menetetty.

Tehtaasta ulostuleville yksiköille voi määrätä sijainnin johon ne ensimmäisenä liikkuvat klikkaamalla haluttua sijaintia oikealla hiiren napilla tehtaan ollessa valittuna (ja tässä taitaa olla myös bugi jossa ko. sijainti asetetaan jos raahaat ruudun sijaintia oikea nappi pohjassa?)


## Resurssit
Kaivosautosi keräävät kaivoksesta malmia joka toimii pelin rahayksikkönä. Rakentaessa Resource Yardin, saat yhden kaivosauton sen mukana, kaivosautoja voi tosin rakentaa lisää War Factoryssa. Osa rakennuksista vaatii toimiakseen sähköä jota saa sähkövoimaloista, huolehdi että sinulla on riittävästi sähköä tai rakennuksesi muuttuvat harmaiksi ja lakkaavat suorittamasta toimintojaan.


## Muuta
Pelissä on joitakin dokumentoimattomia debug-näppäimiä, hitaammille koneille/näytönohjaimille apua lienee esim. `n` & `m` -napeista, joilla voi muuttaa maaston tarkkuutta (n pienentää, m suurentaa, pelin alkaessa maasto on maksimitarkkuudella). Backspace resetoi kameran asennon, zoomin ja sijainnin jos pääset eksymään.


## Tunnetut bugit:
  * Osa toiminnoista voi laueta "dragatessa" (hiirellä näkymää raahatessa)
  * Talon pohjapinta-alaa näyttävä valintaruudukko ei päivity oikein jos vaihtaa toiseen rakennustyyppiä peruuttamatta rakentamista ensin.
  * Nuke-tankkien ja -siilon räjähdysten aiheuttamat kraaterit saattavat katkaista pelialueen kahdeksi erilliseksi mantereeksi, jolloin yksiköt eivät pääse enää vihollisen kimppuun. Lisäksi rakennukset saattavat jäädä kraatereiden ylle leijumaan, mikäli ne eivät tuhoudu räjähdyksessä.


## Yksiköt

<table>
	<tr>
		<td>Taisteluauto (Combat Car)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_1.png" alt="icon" /></td>
		<td>Heikko mutta nopea auto tiedustelukäyttöön.</td>
	</tr>
	<tr>
		<td>Tankki (Tank)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_2.png" alt="icon" /></td>
		<td>Armeijasi perustyökalu.</td>
	</tr>
	<tr>
		<td>Raketinheitinauto (Missile Tank)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_4.png" alt="icon" /></td>
		<td>Hitaasti siirtyvä pitkän kantaman ase.</td>
	</tr>
	<tr>
		<td>Kaivoskuorma-auto (Mining truck)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_6.png" alt="icon" /></td>
		<td>Aseeton ajoneuvo malmin louhintaan.</td>
	</tr>
	<tr>
		<td>Supertankki (Super Tank)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_3.png" alt="icon" /></td>
		<td>Sitkeä ja tulivoimainen. Vaatii Tiedekeskus-rakennuksen</td>
	</tr>
	<tr>
		<td>Raskas heitin (Nuke cannon)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_5.png" alt="icon" /></td>
		<td>Suuritulivoimainen mutta helposti särkyvä pitkän kantaman ase. Vaatii Tiedekeskuksen</td>
	</tr>
</table>


## Rakennukset

<table>
	<tr>
		<td>Voimala (Power Plant)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_54.png" alt="icon" /></td>
		<td>Useimmat rakennukset tarvitsevat sähköä toimiakseen, tämä voimala tuottaa sitä.</td>
	</tr>
	<tr>
		<td>Käsittelylaitos (Resource Yard)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_53.png" alt="icon" /></td>
		<td>Jalostaa kaivoksesta louhutun malmin rahaksi. Käsittelylaitoksen rakentaminen pelin alkumetreillä on ensiarvoisen tärkeää, jotta saat lisää resursseja.</td>
	</tr>
	<tr>
		<td>Tehdas (War Factory)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_52.png" alt="icon" /></td>
		<td>Valmistaa erilaisia yksiköitä.</td>
	</tr>
	<tr>
		<td>Tutka-asema (Radar building)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_56.png" alt="icon" /></td>
		<td>Kun rakennat tutka-aseman saat käyttöösi hyödyllisen tutkanäkymän josta näet koko taistelukentän kerralla, jos vain sähkön tuotantosi riittää.</td>
	</tr>
	<tr>
		<td>Tiedekeskus (Science Building)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_57.png" alt="icon" /></td>
		<td>Jotta voit rakentaa kehittyneemmän teknologian rakennuksia ja yksiköitä tarvitset ensin tiedekeskuksen.</td>
	</tr>
	<tr>
		<td>Tykkitorni (Gun tower)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_59.png" alt="icon" /></td>
		<td>Pienitehoinen tykkitorni puolustuksen tueksi.</td>
	</tr>
	<tr>
		<td>Kaivos (Deep Bore Mine)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_55.png" alt="icon" /></td>
		<td>Malmin louhiminen syvältä maan sisältä on hidasta puuhaa mutta jos vihollinen estää pääsyn pinnalla oleviin kaivoksiin se saattaa olla välttämätöntä. Tällainen kaivos tuottaa hitaasti malmia, jos vain sähköä on riittävästi saatavilla. Vaatii Tiedekeskuksen.</td>
	</tr>
	<tr>
		<td>Isompi tykkitorni (Cannon tower)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_60.png" alt="icon" /></td>
		<td>Parempi tykkitorni puolustuksen tueksi. Vaatii Tiedekeskuksen</td>
	</tr>
	<tr>
		<td>Ohjussiilo (Nuke Silo)</td>
		<td><img class="icon" width="128" height="128" src="http://zemm.github.com/mazerts/icons/icon_58.png" alt="icon" /></td>
		<td>Hidas ja kallis mutta erittäin tuhovoimainen joukkotuhoase joka jättää jälkeensä pelkkiä savuavia kraatereita. Siilolla täytyy ampua itse valitsemalla siilo, ja näkyviin ilmestyvän napin klikkaamisen jälkeen voit valita mihin kohtaan siilo ampuu, ampumisen jälkeen siilon täytyy latautua uudelleen (latautuminen näkyy napissa). Vaatii Tiedekeskuksen.</td>
	</tr>
</table>
