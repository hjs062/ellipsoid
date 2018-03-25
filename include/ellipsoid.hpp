#ifndef _JS_GEO_ELLIPSOID_H_HJS_20180319_
#define _JS_GEO_ELLIPSOID_H_HJS_20180319_

#include <cstdint>
#include <cmath>

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace JS
{
    namespace Geo
    {
        // https://en.wikipedia.org/wiki/Ellipsoid
        // x*x   y*y   z*z
        // --- + --- + --- = 1
        // a*a   b*b   c*c
        class Ellipsoid
        {
        public:
            Ellipsoid(const double & _a, const double & _b, const double & _c)
            : a(_a), b(_b), c(_c)
            {
            }
            
            glm::dvec3 getPosition(const double & lon, const double & lat)
            {
                const double & rad_lon = glm::radians(lon);
                const double & rad_lat = glm::radians(lat);
                return glm::vec3(
                                 a * cos(rad_lat) * cos(rad_lon),
                                 b * cos(rad_lat) * sin(rad_lon),
                                 c * sin(rad_lat)
                                 );
            }
            
            void getPositionsBySampling(std::vector<glm::vec3> & vecPosition, const unsigned int & sample_size_on_xy, const unsigned int & sample_size_on_yz)
            {
                const double & degree_on_xy = 90.0 / sample_size_on_xy;
                const double & degree_on_yz = 90.0 / sample_size_on_yz;
                
                const unsigned int & size_of_points = (1 + 4 * sample_size_on_xy) * (1 + 2 * sample_size_on_yz);
                
                vecPosition.clear();
                vecPosition.reserve(size_of_points);
                
                for(int j = 0; j < (1 + 2 * sample_size_on_yz); ++j)
                {
                    for(int i = 0; i < (1 + 4 * sample_size_on_xy); ++i)
                    {
                        const double & lon = i * degree_on_xy;
                        const double & lat = 90.0 - j * degree_on_yz;
                        vecPosition.push_back(getPosition(lon, lat));
                    }
                }
            }
            
        protected:
            double a, b, c;
        };
    }
}

#endif
