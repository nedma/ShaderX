// Tone mapping operator on PS 3.0 devices

// sample the high dynamic range texture
texture HDRTexture;
sampler TexToToneMap = sampler_state 
{
	Texture = (HDRTexture);
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

texture BarLwTex;
sampler BarLwSampler = sampler_state 
{
	Texture = (BarLwTex);
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

float KeyA = 0.18;
float RecipLwhiteSqrd = 1.f / 1e20f; // very large number

float4 ToneMapHDRToLDR( float2 uv : TEXCOORD0 ): COLOR0
{
   // get average Lw from the texture we calculated previously
   float BarLw = tex2D( BarLwSampler, float2(0,0) );
   float aOverBarLw = KeyA * BarLw;

   // calc L(x,y) by getting the Luminance of this pixel and
   // scaling by a/BarLw
   float4 hdrPixel = tex2D( TexToToneMap, uv );
   float lumi = dot( hdrPixel, float3( 0.27, 0.67, 0.06 ) ) + 0.00005f;
   float Lxy = aOverBarLw * lumi;
   
   // now calculate Ld
   float numer = (1 + (Lxy * RecipLwhiteSqrd)) * Lxy;
   float denum = 1 + Lxy;
   float Ld = numer / denum;
   
   // we now have to convert the original colour into this range.
   float3 ldrPixel = (hdrPixel / lumi) * Ld;
   
   // currently don't process hdr alpha just saturate on the output to LDR
   return float4(ldrPixel, hdrPixel.a);

}

#define CODE_TO_REPEAT1     texld r1.xyz, r0, s0	\
                            dp4 r2.x, r1.xyzw, c3.xyzw 	\
                            log r2.x, r2.x		\
                            add r0.xw, r0.xwww, r2.wxxx

#define CODE_TO_REPEAT5    CODE_TO_REPEAT1\
                            CODE_TO_REPEAT1\
                            CODE_TO_REPEAT1\
                            CODE_TO_REPEAT1\
                            CODE_TO_REPEAT1
#define CODE_TO_REPEAT25 CODE_TO_REPEAT5\
                            CODE_TO_REPEAT5\
                            CODE_TO_REPEAT5\
                            CODE_TO_REPEAT5\
                            CODE_TO_REPEAT5
#define CODE_TO_REPEAT125  CODE_TO_REPEAT25\
                            CODE_TO_REPEAT25\
                            CODE_TO_REPEAT25\
                            CODE_TO_REPEAT25\
                            CODE_TO_REPEAT25



// two assembler methods for generating mean average log luminance
// single pass version
technique T0
{
	pass P0
	{
        PixelShader = ASM {
			ps_3_0
			dcl_texcoord0     v0.xy
			dcl_2d s0
			def c3, 0.27f, 0.67f, 0.006f, 0.0005f

			// v0.xy = start uv co-ordinate for this block		Instruction count
			// c0.z = 0
			// c1.xyzw = xy = delta uv for each pixel, z = 0, w = 1
			// s0 = frame buffer sampler
			// c3 = 0.27, 0.67, 0.06, sqrt(delta)
			mov r0.xy, v0.xy	// set uv to start of this block		1
			mov r0.zw, c0.z	// clear accumulator				1
			
			mov r1.w, c3.w	// set w = sqrt(delta) so r1.w * r1.w = delta	1
			mov r2.w, c1.x	// optimise the inner loop			1

			rep i4		// loop across yblocks (we have to do this because i loops are restricted to 255 iterations)
				rep i2		// loop across y				3
					rep i3			// modification to make this single pass on PS 3.0 shader that have even more instructions 
						CODE_TO_REPEAT125
					endrep		// end x block loop
					mad r0.xy, r0.xy, c1.zw, c1.zy
				endrep		// end y loop				2
			endrep
					
			mul r0.x, r0.w, c0.x	// multiple by 1/N
			exp r0.x, r0.x	// and convert back into a luminance value
			rcp r0.x, r0.x // take the reciprical when generated the mean instead of at every pixel when using it...
			mov oC0.xyzw, r0.xxxx
			};	
	}
	
	pass P1
	{
		PixelShader = compile ps_2_0 ToneMapHDRToLDR();
	}
}


// 2 pass version
technique T1
{
	pass P0
	{
        PixelShader = ASM {
			ps_3_0
			dcl_texcoord0     v0.xy
			dcl_2d s0
			def c0, 0,0,0,0
			def c3, 0.27f, 0.67f, 0.006f, 0.0005f

			// v0.xy = start uv co-ordinate for this block		Instruction count
			// c0.z = 0
			// c1.xyzw = xy = delta uv for each pixel, z = 0, w = 1
			// s0 = frame buffer sampler
			// c3 = 0.27, 0.67, 0.06, sqrt(delta)
			mov r0.xy, v0.xy	// set uv to start of this block		1
			mov r0.zw, c0.z	// clear accumulator				1
			
			mov r1.w, c3.w	// set w = sqrt(delta) so r1.w * r1.w = delta	1
			mov r2.w, c1.x	// optimise the inner loop			1

			rep i2		// loop across y				3
				CODE_TO_REPEAT125
				mad r0.xy, r0.xy, c1.zw, c1.zy
			endrep		// end y loop				2
					
			mov oC0.xyzw, r0.wwww
			};	
	}
	pass P1
	{
        PixelShader = ASM {
			ps_3_0
			dcl_texcoord0     v0.xy
			dcl_2d s0

			// c0.x = 1/N
			// c0.z = 0
			// c1.xyzw = xy = delta uv for each pixel, z = 0, w = 1
			// s0 = frame buffer sampler
			mov r0.xy, v0.xy	// set uv to start of this block	
			mov r0.zw, c0.z	// clear accumulator			
			mov r2.w, c1.x	// u delta
			rep i4						// ysize = 8 in the example above
				rep i3				// xsize = 11 in the example above
					texld r2.x, r0, s0	// get the pixel from the last pass	
					add r0.xw, r0.xwww, r2.wxxx	// Sum log-luminance and inc u
				endrep		// end x loop
				mad r0.xy, r0.xy, c1.zw, c1.zy	// increment v and set u to 0
			endrep		// end y loop
			mul r0.x, r0.w, c0.x	// multiple by 1/N
			exp r0.x, r0.x	// and convert back into a luminance value
			rcp r0.x, r0.x // take the reciprical when generated the mean instead of at every pixel when using it...
			mov oC0.xyzw, r0.xxxx
		};
	}
	pass P1
	{
		PixelShader = compile ps_2_0 ToneMapHDRToLDR();
	}
}
