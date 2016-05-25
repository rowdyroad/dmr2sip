'use strict'
angular.module('ac.directives',[]);
angular.module('ac.serivces',[]);


angular.module('ac', [
  'angular-growl',
  'textAngular',
  'ngRoute',
  'angular-loading-bar',

  'ngTagsInput',
  'ngAnimate',
  'ngAria',
  'ngMessages',
  'ngtimeago',

  // Custom modules
  'ac.i18n',

  // 3rd Party Modules
  'ngMaterial',
  'ui.bootstrap',
  'duScroll',

  'ac.controllers',
  'ac.services',
  'ac.directives'

])
.config(function($mdThemingProvider) {
        var cyanAlt = $mdThemingProvider.extendPalette('cyan', {
            'contrastLightColors': '500 600 700 800 900',
            'contrastStrongLightColors': '500 600 700 800 900'
        })
        var lightGreenAlt = $mdThemingProvider.extendPalette('light-green', {
            'contrastLightColors': '500 600 700 800 900',
            'contrastStrongLightColors': '500 600 700 800 900'
        })

        $mdThemingProvider
            .definePalette('cyanAlt', cyanAlt)
            .definePalette('lightGreenAlt', lightGreenAlt);


        $mdThemingProvider.theme('default')
            .primaryPalette('teal', {
                'default': '500'
            })
            .accentPalette('cyanAlt', {
                'default': '500'
            })
            .warnPalette('red', {
                'default': '500'
            })
            .backgroundPalette('grey');
})
.config(function($httpProvider,$locationProvider,$sceProvider) {
  $sceProvider.enabled(false);
  $locationProvider.html5Mode(true);
  $httpProvider.defaults.headers.common["X-Requested-With"] = 'XMLHttpRequest';
     $httpProvider.interceptors.push(function($q, $location, $rootScope,growl) {
      return {
        responseError: function(response) {
          if (response.status == 403) {
            return $q.reject(response);
          }

          var k = "<b>" + response.statusText+'</b>';
          if (response.status == 422) {
              k +='<ul>';
              for (var i in response.data) {
                  k+='<li>' + response.data[i].message + '</li>';
              }
              k+='</ul>';
          }
          return $q.reject(response);
        }
    }});
})
.run(function($location, $route, $rootScope) {
    var original = $location.path;
    $location.path = function (path, reload) {
        if (reload === false) {
            var lastRoute = $route.current;
            var un = $rootScope.$on('$locationChangeSuccess', function () {
                $route.current = lastRoute;
                un();
            });
        }
        return original.apply($location, [path]);
    };
});

