#version 330 compatibility

out vec3 gEyePosition;// eye-space position
out vec3 gNormal;
vec3 gModelPosition;	
out vec3 T;

uniform sampler2D bvh_sampler;
//radius of the tool head
uniform float radius = 1.5f;
uniform int maxIndex = 275210;
//orientation of the tool
uniform vec3 orientation = vec3(0, 0, 1);
uniform float bvh_tex_size = 800;
float maxFloat = 3.40282e+38;



bool isInside(vec3 boxMin, vec3 boxMax)
{
	return
		gModelPosition.x <= boxMax.x &&
		gModelPosition.y <= boxMax.y &&
		gModelPosition.z <= boxMax.z &&
		gModelPosition.x >= boxMin.x &&
		gModelPosition.y >= boxMin.y &&
		gModelPosition.z >= boxMin.z;
}



vec3 computeNormal()
{
	vec3 closestPoint;
	vec3 normal = vec3(0, 0, 1);
	float minLength = maxFloat;
	
	int loopCount = 5;
	
	int idx = 0;
	while (idx < maxIndex && loopCount > 0)
	{
		vec4 data1 = texelFetch(bvh_sampler, ivec2( int(mod(idx, bvh_tex_size)), int(floor(idx/bvh_tex_size)) ), 0);
		idx++;
		vec4 data2 = texelFetch(bvh_sampler, ivec2( int(mod(idx, bvh_tex_size)), int(floor(idx/bvh_tex_size)) ), 0);
		idx++;
		
		if (data1.w > 0) // current element is a bbox
		{
			if ( !isInside(data1.xyz, data2.xyz) ) idx = int(data2.w);//escape
		}
		else
		{
		
			vec3 beginMove = data1.rgb;
			vec3 endMove = data2.rgb;
			
			// Move toolpath to center of tool
			beginMove += orientation * radius;
			endMove += orientation * radius;

			// Calculate normal
			vec3 moveVec = endMove - beginMove;
			vec3 beginToPos = gModelPosition - beginMove;

			float projection;
			float moveLength = sqrt(dot(moveVec, moveVec));
			if( moveLength == 0.0f )
			{
				projection = 0.0f;
			}
			else
			{
				moveVec /= (moveLength);
				projection = (dot(beginToPos, moveVec));
			}

			if (projection < 0)
			{
				closestPoint = beginMove;
			}
			else if ( projection > moveLength)
			{
				closestPoint = endMove;
			}
			else
			{
				closestPoint = beginMove + projection * moveVec;
			}

			vec3 dist = closestPoint - gModelPosition;
			float squaredDist = dot(dist, dist);
			
			if(squaredDist < minLength)
			{
				normal = dist;
				minLength = squaredDist;
				T = moveVec;
			}
		}
		loopCount--;
	}
		
	return gl_NormalMatrix * (normal);
}



void main() {

	//model position
	gModelPosition = vec3(gl_Vertex);
    gNormal = normalize(gl_NormalMatrix * gl_Normal);
	if (dot( gNormal, (gl_NormalMatrix * orientation) ) > 0.00001) gNormal = normalize(computeNormal());

    // eye space position:
    gEyePosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	//vColor = gl_Color;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

