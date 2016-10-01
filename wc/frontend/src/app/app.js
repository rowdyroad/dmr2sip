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
  'ac.directives',
  'pascalprecht.translate',
  'ngCookies'

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
.config(function($httpProvider,$locationProvider,$sceProvider,$translateProvider) {
  $sceProvider.enabled(false);
  $locationProvider.html5Mode(true);
  $httpProvider.defaults.headers.common["X-Requested-With"] = 'XMLHttpRequest';
     $httpProvider.interceptors.push(function($q, $location, $rootScope,growl) {
      return {
        responseError: function(response) {
          if (response.status == 403 || response.status == 401) {
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

          growl.error(k, {ttl: 3000});

          return $q.reject(response);
        }
    }});

     $translateProvider.useStaticFilesLoader({
          prefix: 'i18n/',
          suffix: '.json'
      });

      $translateProvider.
      registerAvailableLanguageKeys(['en', 'ru'],
      {
          'en_*': 'en',
          'ru_*': 'ru',
          '*': 'en'
      }).
      determinePreferredLanguage();

})
.run(function($location, $route, $rootScope, $translate) {

    $rootScope.$on('$translateChangeSuccess', function(event, data) {
        if (!localStorage.getItem('language')) {
            localStorage.setItem('language', data.language);
        }
        $rootScope.language = localStorage.getItem('language');
    });

    $rootScope.setLanguage = function(lang)
    {
      localStorage.setItem('language', lang);
      $translate.use(lang);
    }



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

