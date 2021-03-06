#include "MobileTextures.h"
#include "game_sa\CTxdStore.h"
#include "game_sa\CFileLoader.h"
#include "game_sa\CTimer.h"
#include "MobileText.h"

MobileTextures mobileTex;
MobileTexDictionary MobileTextures::m_menuSliderTxd, MobileTextures::m_aSplashesTxd, MobileTextures::m_nBackgroundSpriteTxd;//MobileTextures::m_weaponicons,



void MobileTexDictionary::Load() {
	
    if (!m_pRwTexDictionary) {
        m_pRwTexDictionary = CFileLoader::LoadTexDictionary(m_szPath);
        if (m_pRwTexDictionary) {
            RwTexDictionaryForAllTextures(m_pRwTexDictionary, [](RwTexture *tex, void *data) {
                reinterpret_cast<std::vector<RwTexture *> *>(data)->push_back(tex);
                return tex;
            }, &m_textures);
        }
        m_nLastUsedTime = CTimer::m_snTimeInMillisecondsNonClipped;
    }
}

void MobileTexDictionary::Unload() {
    if (m_pRwTexDictionary) {
        RwTexDictionaryDestroy(m_pRwTexDictionary);
        m_pRwTexDictionary = nullptr;
        m_textures.clear();
    }
}

void MobileTexDictionary::AddToStorage() {
    MobileTxdStorage::Instance().AddTxd(this);
}

MobileTexDictionary::MobileTexDictionary() {
    m_szPath[0] = '\0';
    m_bInitialised = false;
    m_nLastUsedTime = 0;
}

void MobileTexDictionary::Init(char *path) {
    strcpy(m_szPath, path);
    m_pRwTexDictionary = nullptr;
    m_bInitialised = true;
    AddToStorage();
}

RwTexture *MobileTexDictionary::GetTexture(unsigned int id) {
	if (m_bInitialised) {
		Load();
		if (m_textures.size() > id) {
			m_nLastUsedTime = CTimer::m_snTimeInMillisecondsNonClipped;
			return m_textures[id];
		}
	}
	return nullptr;
}

void MobileTxdStorage::Process() {
    for (auto txd : m_txds) {
        if (CTimer::m_snTimeInMilliseconds - txd->m_nLastUsedTime > LOAD_UNLOAD_TIME)
            txd->Unload();
    }
}

void MobileTxdStorage::AddTxd(MobileTexDictionary *txd) {
    m_txds.push_back(txd);
}

void MobileTxdStorage::Shutdown() {
    for (auto txd : m_txds) {
        txd->Unload();
    }
}

MobileTxdStorage &MobileTxdStorage::Instance() {
    static MobileTxdStorage storage;
    return storage;
}

RwTexture *MobileTextures::LoadTexture(char *path)
{
	RwInt32 width, height, depth, flags;
	RwImage *image = RtPNGImageRead(reinterpret_cast<RwChar *>(path));
	RwImageFindRasterFormat(image, 4, &width, &height, &depth, &flags);
	RwRaster *raster = RwRasterCreate(width, height, depth, flags);
	RwRasterSetFromImage(raster, image);
	RwImageDestroy(image);
	return RwTextureCreate(raster);
}
//#include "Settings.h"
//#define CALLVOID(a) ((void (__cdecl *)())a)()

void MobileTextures::Setup() {
    m_menuSliderTxd.Init(PLUGIN_PATH("IronMan\\textures/menu_slider.txd"));
    m_aSplashesTxd.Init(PLUGIN_PATH("IronMan\\textures/maxirpscs.txd"));
    m_nBackgroundSpriteTxd.Init(PLUGIN_PATH("IronMan\\textures/menu_background.txd"));
	//readpngs();
}

