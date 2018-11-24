Sample for ShaderX 3 Deferred Lighting on PS 3.0 with High Dynamic Range

This sample demonstrates the per-pixel material and HDR for a deferred lighting system.
The per-pixel material is used on the floor to create a 'blood shadow' effect. This material has the strange effect on whenever shadow falls on it, it turns a red colour. The shadow fall on the rest of the scene normally except for center teapot which sharpens the shadow outline.
The HDR is switched demonstrated via the main directional lighting being dimmed every 5 frames (very fast if you have hardware but still fairly slow if you are using the REFRAST to view this sample). The tone mapping adapts the visual range to accomadate (you will notice several light source 'appear' when the light is dimmed, they were allows there but there contribution to the lighting scene isn't enough to be visible with the main light at full blast). 

As mentioned in the article there are two tone-mapping, one for system with restricted instruction lengths, another for virtually unrestricted instruction lengths. The first (Technique "T1") (used on NV40 currently) takes two passes to calculate the mean average log luminance of a scene, the other does it in a single pass (Technique "T0").

There is alot of 'support' code, this is due to this being my experimental deferred lighting architecture. For a full description of the base architecture see http://www.beyond3d.com/articles/deflight/ 

Key Files for this article:
ToneMap.fx : Contains the FX file to do tonemapping on PS 3.0
LightDirection.fx : Has the per-pixel material code
deferedHDR.cpp : The main driver code, has the setup code and main loop.

Note: The article nor the demo doesn't demonstrate some things that tend to get grouped with tone-mapping (but aren't). There is no guassian bloom filter (the filter that cause specular to 'bleed' over) or slow light adaption (the tone-mapping changes instantly, a real system would lerp with existing values so the simulated eye system takes time to adapt to the new light condition). Also many tone-mapping system, lower the screen resolution before calculating the mean log-luminance, this demo does NOT do this it calculates it at full screen resolution, While there is some speed gains from doing this, the results are not the same so be careful (you need to filter in log space not linear space to get same results).


LEGAL:
	This sample (all source code etc) comes with no warrenty implied or otherwise. You can use and abuse it as you wish. It is public domain, completely free in all senses of the word.

Bye,
	Deano

Dean Calver

EMail: deano@rattie.demon.co.uk
