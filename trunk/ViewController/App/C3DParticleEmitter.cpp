#include "C3DParticleEmitter.h"
#include "IApplication.h"	// only for RandFloat function
#include "C3DObject.h"
#include "../Camera/FrustumCull.h"

// bitmasks for the emitter effects
#define EMITTERFLAGS_SCALE				1
#define EMITTERFLAGS_FADE				2
#define EMITTERFLAGS_ROTATE				4


D3DXVECTOR3 C3DParticleEmitter::partAABBMin(-1.0f, -1.0f, -1.0f);
D3DXVECTOR3 C3DParticleEmitter::partAABBMax(1.0f, 1.0f, 1.0f);

C3DParticleEmitter::C3DParticleEmitter(float lifeTime)
{
	m_dwNumTextures = 0;
	m_eType = ePARTICLETYPE_BILLBOARD;
	m_ppTextures = NULL;
	m_dwFlags = 0;
    m_pParticleList = NULL;
	//m_pObject = NULL;
	//::memset(&m_ObjectMaterial, 0, sizeof(D3DMATERIAL9));

    m_LifeTime = lifeTime;

    if(lifeTime > 0)
    {
        m_NeverDies = false;
    }
    else
    {
        m_NeverDies = true;
    }
}


C3DParticleEmitter::~C3DParticleEmitter(void)
{
}

HRESULT C3DParticleEmitter::Create(	DWORD dwNumTextures,
									LPDIRECT3DTEXTURE9* ppTextures,
									ePARTICLETYPE eType)
{
    m_pParticleList = new DoubleLinkedList<PARTICLE*>();

	if (dwNumTextures && ppTextures)
	{
		// allocate array of texture pointers
		m_ppTextures = new LPDIRECT3DTEXTURE9[dwNumTextures];
		if (!m_ppTextures)
		{
			return E_OUTOFMEMORY;
		}

		// copy texture pointers to internal array
		DWORD i;
		for (i=0; i<dwNumTextures; i++)
		{
			m_ppTextures[i] = ppTextures[i];
		}
	}
	m_dwNumTextures = dwNumTextures;
	m_eType = eType;

	//InitMatrices();
	return S_OK;
}


void C3DParticleEmitter::Release(void)
{
	// release particles
    if(m_pParticleList)
    {
        ListNode<PARTICLE*>* node = m_pParticleList->headNode();    
        while(node)
        {
            delete node->item;
            node = node->next;
        }        
        delete m_pParticleList;
        m_pParticleList = NULL;
    }

	// release texture pointer array
	if (m_ppTextures)
	{
		delete [] m_ppTextures;
		m_ppTextures = NULL;
	}
	m_dwNumTextures = 0;

	I3DObject::Release();
}


bool C3DParticleEmitter::Update(float fFrametime)
{
	if (IsActive())
	{
        // update particle 3d object
		/*if (m_pObject)
		{
			m_pObject->Update(fFrametime);
		}*/

		// update particle positions
        ListNode<PARTICLE*>* node = m_pParticleList->headNode();
        while(node)
        {
            
			PARTICLE* particle = node->item;

			// move the particle
			particle->vPosition.x += particle->vDirection.x * fFrametime;
			particle->vPosition.y += particle->vDirection.y * fFrametime;
			particle->vPosition.z += particle->vDirection.z * fFrametime;

			// decrease the lifetime
			particle->fLifeTime -= fFrametime;

			// check if particle is dead
			if (particle->fLifeTime < 0.0f)
			{
				// particle is dead, remove it
                delete particle;
                node = m_pParticleList->removeGetNext(node);
			}
            else
            {
                node = node->next;
            }
		}

		// update rotation matrices
/*		if ((m_dwFlags & EMITTERFLAGS_ROTATE) &&
			m_eType == ePARTICLETYPE_OBJECT)
		{
			DWORD i, j;
			for (i=0; i<NUM_ROTATIONMATRICES; i++)
			{
				for (j=0; j<3; j++)
				{
					// increase rotation angles by delta
					m_fRotationAngle[i][j] +=
						m_fRotationDelta[i][j] * fFrametime;
				}

				// build rotation matrix
				D3DXMatrixRotationYawPitchRoll(	&m_RotationMatrix[i],
												m_fRotationAngle[i][0],
												m_fRotationAngle[i][1],
												m_fRotationAngle[i][2]);
			}
		}*/
	}

	// update children
	I3DObject::Update(fFrametime);

    if(!m_NeverDies)
    {
        m_LifeTime -= fFrametime;
        if(m_LifeTime < 0)
        {
            return false;
        }
    }

    return true;
}


void C3DParticleEmitter::Render(LPDIRECT3DDEVICE9 pDevice)
{

    D3DXVECTOR3 minAABB(m_mWorld._41 + partAABBMin.x,
                        m_mWorld._42 + partAABBMin.y,
                        m_mWorld._43 + partAABBMin.z);
    D3DXVECTOR3 maxAABB(m_mWorld._41 + partAABBMax.x,
                        m_mWorld._42 + partAABBMax.y,
                        m_mWorld._43 + partAABBMax.z);
        
    if((m_pParticleList->headNode() != NULL) && (FrustumCull::cullAABB(minAABB, maxAABB)))
    {
	    // set identity matrix as a world transformation
	    D3DXMATRIX m;
	    D3DXMATRIX mScale;
	    D3DXMatrixIdentity(&m);
	    pDevice->SetTransform(D3DTS_WORLD, &m);


	    float fPhase = 0.0f;
	    float fTemp = 0.0f;
	    LPDIRECT3DTEXTURE9 pLastTexture = NULL;
	    POINTSPRITEVERTEX sprite;
	    D3DXCOLOR color;

	    if (m_eType == ePARTICLETYPE_BILLBOARD && IsVisible())
	    {
		    // set the 3d device for the point sprite rendering
		    pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		    pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		    pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
		    pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);

		    pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&fTemp));
		    pDevice->SetRenderState(D3DRS_POINTSCALE_A, *((DWORD*)&fTemp));
		    pDevice->SetRenderState(D3DRS_POINTSCALE_B, *((DWORD*)&fTemp));
		    fTemp = 1.0f;
		    pDevice->SetRenderState(D3DRS_POINTSCALE_C, *((DWORD*)&fTemp));

		    // set FVF flags
		    pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

		    // if we have only one texture, set it to device
            if (m_dwNumTextures == 1 && m_ppTextures[0] != C3DObject::getCurrentTexture())
		    {
			    pDevice->SetTexture(0, m_ppTextures[0]);
                C3DObject::setCurrentTexture(m_ppTextures[0]);
		    }



		    // render the particles
            ListNode<PARTICLE*>* node = m_pParticleList->headNode();
            while(node)
            {
                
			    PARTICLE& particle = *node->item;

			    // compute particle life time phase
			    // 0.0 at the beginning of life
			    // 1.0 at the end
			    fPhase = particle.fLifeTime / particle.fMaxLifeTime;

			    if (m_dwNumTextures > 1)
			    {
				    // compute right texture index
				    DWORD dwIndex = (DWORD)( (1.0f - fPhase) * (float)(m_dwNumTextures - 1));

				    // set the texture into the device, but only
				    // if this is really a different from current one
				    if (m_ppTextures[dwIndex] != pLastTexture)
				    {
					    pDevice->SetTexture(0, m_ppTextures[dwIndex]);
					    pLastTexture = m_ppTextures[dwIndex];
                        C3DObject::setCurrentTexture(pLastTexture);
				    }
			    }

			    // build point sprite vertex
			    sprite.vPosition = particle.vPosition;
			    color = particle.cColor;

			    // apply fade
			    if ((m_dwFlags & EMITTERFLAGS_FADE))
			    {
				    color.r *= fPhase;
				    color.g *= fPhase;
				    color.b *= fPhase;
				    color.a *= fPhase;
			    }
			    sprite.dwColor = color;

			    // set particle size
			    fTemp = particle.fSize;
			    if ((m_dwFlags & EMITTERFLAGS_SCALE))
			    {
				    fTemp *= fPhase;
			    }
			    pDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&fTemp));

			    pDevice->DrawPrimitiveUP(	D3DPT_POINTLIST,
										    1,
										    &sprite,
										    16);

                node = node->next;
		    }

		    // restore some basic settings to 3d device
		    pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		    pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	    }
	    /*else if (m_eType == ePARTICLETYPE_OBJECT && IsVisible())
	    {
		    // render object particles
		    pDevice->SetTexture(0, NULL);

		    if ((m_dwFlags & EMITTERFLAGS_SCALE))
		    {
			    // enable automatic vertex normal normalization
			    // since scaling matrix will also scale normals and
			    // distord the lighting
			    pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		    }

            ListNode<PARTICLE*>* node = m_pParticleList->headNode();
            while(node)
            {
                
			    PARTICLE& particle = *node->item;

			    // compute particle phase
			    fPhase = particle.fLifeTime / particle.fMaxLifeTime;

			    // build particle matrix
			    if ((m_dwFlags & EMITTERFLAGS_ROTATE))
			    {
				    ::memcpy(	&m,
							    &m_RotationMatrix[particle.dwMatrixIndex],
							    sizeof(D3DXMATRIX));
			    }
			    else
			    {
				    D3DXMatrixIdentity(&m);
			    }

			    if ((m_dwFlags & EMITTERFLAGS_SCALE))
			    {
				    if ((m_dwFlags & EMITTERFLAGS_ROTATE))
				    {
					    // if we have both, scaling and rotates on,
					    // we need to create scaling matrix and
					    // multiply it with rotations to create correct
					    // transformation
					    D3DXMatrixScaling(&mScale, fPhase, fPhase, fPhase);
					    D3DXMatrixMultiply(&m, &m, &mScale);
				    }
				    else
				    {
					    // no rotations, we can apply scaling just
					    // by multiplying the diagonal of the matrix
					    m._11 *= fPhase;
					    m._22 *= fPhase;
					    m._33 *= fPhase;
				    }
			    }

			    // set particle position into the matrix
			    m._41 = particle.vPosition.x;
			    m._42 = particle.vPosition.y;
			    m._43 = particle.vPosition.z;

			    // set particle color
			    m_ObjectMaterial.Diffuse = particle.cColor;

			    // apply fade
			    if ((m_dwFlags & EMITTERFLAGS_FADE))
			    {
				    m_ObjectMaterial.Diffuse.r *= fPhase;
				    m_ObjectMaterial.Diffuse.g *= fPhase;
				    m_ObjectMaterial.Diffuse.b *= fPhase;
				    m_ObjectMaterial.Diffuse.a *= fPhase;
			    }

			    pDevice->SetMaterial(&m_ObjectMaterial);

			    // copy temporary matrix into the objects world matrix
			    // and render the object
			    ::memcpy(&m_pObject->GetWorldMatrix(), &m, sizeof(D3DXMATRIX));
			    m_pObject->Render(pDevice);

                node = node->next;
		    }

		    if ((m_dwFlags & EMITTERFLAGS_SCALE))
		    {
			    // disable vertex normal normalization
			    pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
		    }
	    }*/

        // render the children
        ListNode<I3DObject*>* node = m_arrChildren.headNode();    
        
        while(node)
        {
            node->item->Render(pDevice);
            node = node->next;
        }
    }
}


void C3DParticleEmitter::Emit(const EMIT_PARAMS& params)
{
	DWORD i;
	D3DXVECTOR3 vTemp;

	for (i=0; i<params.dwCount; i++)
	{
		PARTICLE* particle = new PARTICLE();

		// set particle position
		particle->vPosition = params.vPosition;
		particle->vPosition.x +=
			IApplication::RandFloat(-params.vPositionSpread.x, params.vPositionSpread.x);
		particle->vPosition.y +=
			IApplication::RandFloat(-params.vPositionSpread.y, params.vPositionSpread.y);
		particle->vPosition.z +=
			IApplication::RandFloat(-params.vPositionSpread.z, params.vPositionSpread.z);

		// transform particle starting position with emitter
		// world matrix to get starting position relative to emitter
		D3DXVec3TransformCoord(	&particle->vPosition,
								&particle->vPosition,
								&m_mWorld);


		// set particle direction
		particle->vDirection = params.vDirection;
		particle->vDirection.x +=
			IApplication::RandFloat(-params.vDirectionSpread.x, params.vDirectionSpread.x);
		particle->vDirection.y +=
			IApplication::RandFloat(-params.vDirectionSpread.y, params.vDirectionSpread.y);
		particle->vDirection.z +=
			IApplication::RandFloat(-params.vDirectionSpread.z, params.vDirectionSpread.z);

		// transform particle starting direction with emitter
		// world matrix to get starting direction relative to emitter
		D3DXVec3TransformNormal(	&particle->vDirection,
									&particle->vDirection,
									&m_mWorld);

		
		// set particle life time
		particle->fLifeTime = params.fLifeTime;
		particle->fLifeTime +=
			IApplication::RandFloat(-params.fLifeTimeSpread, params.fLifeTimeSpread);
		particle->fMaxLifeTime = particle->fLifeTime;


		// set particle size
		particle->fSize = params.fSize;
		particle->fSize +=
			IApplication::RandFloat(-params.fSizeSpread, params.fSizeSpread);

		// set particle weight
		particle->fWeight = params.fWeight;
		particle->fWeight +=
			IApplication::RandFloat(-params.fWeightSpread, params.fWeightSpread);

		// set particle color
		particle->cColor = params.cColor;
		particle->cColor.r += IApplication::RandFloat(
			-params.cColorSpread.r, params.cColorSpread.r);
		particle->cColor.g += IApplication::RandFloat(
			-params.cColorSpread.g, params.cColorSpread.g);
		particle->cColor.b += IApplication::RandFloat(
			-params.cColorSpread.b, params.cColorSpread.b);
		particle->cColor.a += IApplication::RandFloat(
			-params.cColorSpread.a, params.cColorSpread.a);

		// set random matrix index for each particle
		particle->dwMatrixIndex =
			IApplication::RandInt(0, NUM_ROTATIONMATRICES - 1);


		// add particle into the particle array
		m_pParticleList->pushTail(particle);
	}
}


void C3DParticleEmitter::SetScale(BOOL bScale)
{
	if (bScale)
	{
		// enable scaling
		m_dwFlags |= EMITTERFLAGS_SCALE;
	}
	else
	{
		// disable scaling
		m_dwFlags &= ~(EMITTERFLAGS_SCALE);
	}
}


void C3DParticleEmitter::SetFade(BOOL bFade)
{
	if (bFade)
	{
		// enable fading
		m_dwFlags |= EMITTERFLAGS_FADE;
	}
	else
	{
		// disable fading
		m_dwFlags &= ~(EMITTERFLAGS_FADE);
	}
}


/*void C3DParticleEmitter::SetRotate(BOOL bRotate)
{
	if (bRotate)
	{
		// enable object rotating
		m_dwFlags |= EMITTERFLAGS_ROTATE;
	}
	else
	{
		// disable object rotating
		m_dwFlags &= ~(EMITTERFLAGS_ROTATE);
	}
}*/


/*void C3DParticleEmitter::InitMatrices(void)
{
	DWORD i, j;
	for (i=0; i<NUM_ROTATIONMATRICES; i++)
	{
		for (j=0; j<3; j++)
		{
			m_fRotationAngle[i][j] = 0.0f;
			m_fRotationDelta[i][j] =
				IApplication::RandFloat(-5.0f, 5.0f);
		}

		D3DXMatrixIdentity(&m_RotationMatrix[i]);
	}
}*/




void C3DParticleEmitter::Gravity(const D3DXVECTOR3& vGravity, float fFrametime)
{
	D3DXVECTOR3 vTemp;

	// pre-compute "one frame" gravity vector
	vTemp.x = vGravity.x * fFrametime;
	vTemp.y = vGravity.y * fFrametime;
	vTemp.z = vGravity.z * fFrametime;

    ListNode<PARTICLE*>* node = m_pParticleList->headNode();
    while(node)
    {            
		PARTICLE& particle = *node->item;

		// apply gravity
		particle.vDirection.x += vTemp.x * particle.fWeight;
		particle.vDirection.y += vTemp.y * particle.fWeight;
		particle.vDirection.z += vTemp.z * particle.fWeight;

        node = node->next;
	}
}


void C3DParticleEmitter::Wind(	const D3DXVECTOR3& vPosition,
								const D3DXVECTOR3& vDirection,
								float fRadius,
								float fFrametime)
{
	D3DXVECTOR3 vTemp;
	float fLengthSq, fLength, fTemp;
	const float fRadiusSq = fRadius * fRadius;

    ListNode<PARTICLE*>* node = m_pParticleList->headNode();
    while(node)
    {            
		PARTICLE& particle = *node->item;		

		// compute vector from wind startpoint to particle
		vTemp.x = vPosition.x - particle.vPosition.x;
		vTemp.y = vPosition.y - particle.vPosition.y;
		vTemp.z = vPosition.z - particle.vPosition.z;

		// compute squared length of the vector
		fLengthSq =
			vTemp.x * vTemp.x + vTemp.y * vTemp.y + vTemp.z * vTemp.z;

		if (fLengthSq < 0.000001f)
		{
			// particle is exactly at wind position...
			// just move to next particle...
			continue;
		}

		if (fLengthSq < fRadiusSq)
		{
			// particle is in wind area
			// calculate actual length of the vector and
			// get the inverse value from it to avoid
			// floating point divisions in the future
			fLength = 1.0f / sqrtf(fLengthSq);

			// compute amount of wind applied to particle
			fTemp = 1.0f - (fLength / fRadius);
			fTemp *= particle.fWeight;
			fTemp *= fFrametime;

			// apply the wind
			particle.vDirection.x += vDirection.x * fTemp;
			particle.vDirection.y += vDirection.y * fTemp;
			particle.vDirection.z += vDirection.z * fTemp;
		}
        node = node->next;
	}
}


void C3DParticleEmitter::GravityPoint(	const D3DXVECTOR3& vPosition,
										float fRadius,
										float fPower,
										float fFrametime)
{
	D3DXVECTOR3 vTemp;
	float fLengthSq, fLength, fTemp;
	const float fRadiusSq = fRadius * fRadius;

    ListNode<PARTICLE*>* node = m_pParticleList->headNode();
    while(node)
    {            
		PARTICLE& particle = *node->item;

		// compute vector from wind startpoint to particle
		vTemp.x = vPosition.x - particle.vPosition.x;
		vTemp.y = vPosition.y - particle.vPosition.y;
		vTemp.z = vPosition.z - particle.vPosition.z;

		// compute squared length of the vector
		fLengthSq =
			vTemp.x * vTemp.x + vTemp.y * vTemp.y + vTemp.z * vTemp.z;

		if (fLengthSq < 0.000001f)
		{
			// particle is exactly at gravity position...
			// just move to next particle...
			continue;
		}

		if (fLengthSq < fRadiusSq)
		{
			// particle is in gravity area
			// calculate actual length of the vector and
			// get the inverse value from it to avoid
			// floating point divisions in the future
			fLength = 1.0f / sqrtf(fLengthSq);

			// compute amount of gravity applied to particle
			fTemp = 1.0f - (fLength / fRadius);
			fTemp *= fPower;
			fTemp *= particle.fWeight;
			fTemp *= fFrametime;
			fTemp *= fLength;

			// apply the gravity
			particle.vDirection.x += vTemp.x * fTemp;
			particle.vDirection.y += vTemp.y * fTemp;
			particle.vDirection.z += vTemp.z * fTemp;
		}
        node = node->next;
	}
}


void C3DParticleEmitter::CollisionSpace(const D3DXMATRIX& mPlane,
										float fBounce,
										float fFriction)
{
	D3DXVECTOR3 vTemp, vTemp2;

    ListNode<PARTICLE*>* node = m_pParticleList->headNode();
    while(node)
    {            
		PARTICLE& particle = *node->item;

		// perform change of base to the particle position
		vTemp = particle.vPosition;
		vTemp.x -= mPlane._41;
		vTemp.y -= mPlane._42;
		vTemp.z -= mPlane._43;

		vTemp2.x =	vTemp.x * mPlane._11 +
					vTemp.y * mPlane._12 +
					vTemp.z * mPlane._13;
		vTemp2.y =	vTemp.x * mPlane._21 +
					vTemp.y * mPlane._22 +
					vTemp.z * mPlane._23;
		vTemp2.z =	vTemp.x * mPlane._31 +
					vTemp.y * mPlane._32 +
					vTemp.z * mPlane._33;

		// vTemp2 is now particle position in 'mPlane' point of view
		if (vTemp2.y < 0.0f)
		{
			// particle is 'under' the matrix plane
			// make sure that particle never goes under,
			// so set coordinate y to 0 and transform back
			// to the original coordinate space.
			vTemp2.y = 0.0f;
			D3DXVec3TransformCoord( &particle.vPosition,
									&vTemp2,
									&mPlane);

			// transform particle direction to plane matrix
			vTemp = particle.vDirection;
			vTemp2.x =	vTemp.x * mPlane._11 +
						vTemp.y * mPlane._12 +
						vTemp.z * mPlane._13;
			vTemp2.y =	vTemp.x * mPlane._21 +
						vTemp.y * mPlane._22 +
						vTemp.z * mPlane._23;
			vTemp2.z =	vTemp.x * mPlane._31 +
						vTemp.y * mPlane._32 +
						vTemp.z * mPlane._33;

			// modify the direction
			vTemp2.x *= fFriction;
			vTemp2.z *= fFriction;
			vTemp2.y *= fBounce;

			// transform direction back to original coordinate space
			particle.vDirection.x =	vTemp2.x * mPlane._11 +
									vTemp2.y * mPlane._21 +
									vTemp2.z * mPlane._31;
			particle.vDirection.y =	vTemp2.x * mPlane._12 +
									vTemp2.y * mPlane._22 +
									vTemp2.z * mPlane._32;
			particle.vDirection.z =	vTemp2.x * mPlane._13 +
									vTemp2.y * mPlane._23 +
									vTemp2.z * mPlane._33;

			//particle.dwMatrixIndex =
			//	IApplication::RandInt(0, NUM_ROTATIONMATRICES - 1);
		}
        node->next;
	}
}
