/**
 * C3DParticleEmitter
 *
 * UI Particle-system
 * 
 * $Revision: 181 $
 * $Date: 2009-03-18 01:12:22 +0200 (ke, 18 maalis 2009) $
 * $Id: DoubleLinkedList.h 181 2009-03-17 23:12:22Z zem $
 */

#ifndef __3DPARTICLEEMITTER_H__
#define __3DPARTICLEEMITTER_H__

#include "I3DObject.h"
#include "../../Model/Common/DoubleLinkedList.h"

// this value defines how many individual
// matrices are used to rotate object particles
// more matrices are used, more it seems like each
// particle has its own rotation. Using more matrices
// also requires more computations.
#define NUM_ROTATIONMATRICES		4


class C3DParticleEmitter : public I3DObject
{
public:
	// struct defines one particle
	struct PARTICLE
	{
		D3DXVECTOR3			vPosition;
		D3DXVECTOR3			vDirection;

		float				fLifeTime;
		float				fMaxLifeTime;

		D3DXCOLOR			cColor;
		float				fSize;

		float				fWeight;

		DWORD				dwMatrixIndex;
	};

	// structure defines parameters needed to emit (throw)
	// a set of particles
	struct EMIT_PARAMS
	{
		DWORD			dwCount;

		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vPositionSpread;

		D3DXVECTOR3		vDirection;
		D3DXVECTOR3		vDirectionSpread;

		D3DXCOLOR		cColor;
		D3DXCOLOR		cColorSpread;

		float			fLifeTime;
		float			fLifeTimeSpread;

		float			fSize;
		float			fSizeSpread;

		float			fWeight;
		float			fWeightSpread;
	};

	// particle types
	enum ePARTICLETYPE
	{
		ePARTICLETYPE_BILLBOARD = 0,
		ePARTICLETYPE_OBJECT,
		ePARTICLETYPE_FORCEDWORD = 0xFFFFFFFF
	};

    /**
     * Constructor
     * @param lifeTime Lifetime of emitter as seconds
     */
	C3DParticleEmitter(float lifeTime);


	virtual ~C3DParticleEmitter(void);

	/**
	 * Create
	 * initialise the particle system
	 * @param dwNumTextures number of textures in 'ppTextures' array
	 * @param ppTextures array of texture pointers to use as billboard animation
	 * @param eType emitter type
	 * @return S_OK or an error code
	 */
	HRESULT Create(	DWORD dwNumTextures,
					LPDIRECT3DTEXTURE9* ppTextures,
					ePARTICLETYPE eType);

	/**
	 * Release
	 * release particle emitter
	 */
	void Release(void);

	/**
	 * Update
	 * update the particle system, calculate particle positions
	 * @param fFrametime application frame time
	 */
	virtual bool Update(float fFrametime);

	/**
	 * Render
	 * render the particle system
	 * @param pDevice
	 */
	virtual void Render(LPDIRECT3DDEVICE9 pDevice);

	/**
	 * Emit
	 * emit a set of particles
	 * @param params emit parameters
	 */
	void Emit(const EMIT_PARAMS& params);

	/**
	 * SetScale
	 * @param bScale if TRUE particle size is scaled by lifetime
	 */
	void SetScale(BOOL bScale);

	/**
	 * SetFade
	 * @param bFade if TRUE particle color is faded by lifetime
	 */
	void SetFade(BOOL bFade);

	/**
	 * SetRotate
	 * @param bRotate if TRUE object particles are rotated randomly
	 */
	void SetRotate(BOOL bRotate);


	/**
	 * GetParticleList
	 * @return reference to internal particle list
	 */
	inline DoubleLinkedList<PARTICLE*>* GetParticleList() { return m_pParticleList; }

	/**
	 * SetObject
	 * set the 3d object that is used as a particle
	 * @param pObject object to set
	 */
	inline void SetObject(I3DObject* pObject) { m_pObject = pObject; }

	/**
	 * GetObject
	 * @return pointer to current particle 3d object, or NULL if not used
	 */
	inline I3DObject* GetObject() { return m_pObject; }



	// ------------------------------------------------------------------------
	// PHYSICS SIMULATIONS

	/**
	 * Gravity
	 * Simple gravity physics simulation
	 * @param vGravity direction and strength of the gravity
	 * @param fFrametime
	 */
	void Gravity(const D3DXVECTOR3& vGravity, float fFrametime);

	/**
	 * GravityPoint
	 * apply gravity point simulation
	 * @param vPosition position of the gravity point
	 * @param fRadius radius of gravity effect
	 * @param fPower pulling power of the gravity point
	 * @param fFrametime
	 */
	void GravityPoint(	const D3DXVECTOR3& vPosition,
						float fRadius,
						float fPower,
						float fFrametime);

	/**
	 * Wind
	 * apply wind physics simulation to the particle system
	 * @param vPosition center position of the wind
	 * @param vDirection direction and strength of the wind
	 * @param fRadius radius of the wind area
	 * @param fFrametime
	 */
	void Wind(	const D3DXVECTOR3& vPosition,
				const D3DXVECTOR3& vDirection,
				float fRadius,
				float fFrametime);


	/**
	 * CollisionSpace
	 * compute collision space for the particles
	 * Particles can never be "under" the space specified
	 * by the mPlane matrix
	 * @param mPlane plane matrix
	 * @param fBounce collision bounce multiplier
	 * @param fFriction multiplier to decrease speed while in plane
	 */
	void CollisionSpace(const D3DXMATRIX& mPlane,
						float fBounce,
						float fFriction);


private:
	/**
	 * InitMatrices
	 * initialise random rotation matrices
	 */
	void InitMatrices(void);


	// point sprite vertex type for billboards
	struct POINTSPRITEVERTEX
	{
		D3DXVECTOR3			vPosition;
		DWORD				dwColor;
	};

	// particle type
	ePARTICLETYPE				m_eType;

	// particle array
	DoubleLinkedList<PARTICLE*>* m_pParticleList;

	// object particle members
	I3DObject*					m_pObject;
	D3DMATERIAL9				m_ObjectMaterial;

	// texture members
	LPDIRECT3DTEXTURE9*			m_ppTextures;
	DWORD						m_dwNumTextures;

	// operational flags
	DWORD						m_dwFlags;

	// object rotation members
	D3DXMATRIX					m_RotationMatrix[NUM_ROTATIONMATRICES];
	float						m_fRotationAngle[NUM_ROTATIONMATRICES][3];
	float						m_fRotationDelta[NUM_ROTATIONMATRICES][3];


    //Lifetime of emitter
    float m_LifeTime;
};

#endif	// __3DPARTICLEEMITTER_H__

