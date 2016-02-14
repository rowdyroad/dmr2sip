'use strict'

    angular.module('ac.i18n', ['pascalprecht.translate'])
        .config(function($translateProvider) {
            $translateProvider.useStaticFilesLoader({
                prefix: 'i18n/',
                suffix: '.json'
            });

            $translateProvider.preferredLanguage('en');
            $translateProvider.useSanitizeValueStrategy(null);
        });
/*
    function LangCtrl($scope, $translate) {
        $scope.lang = 'English';
        $scope.setLang = setLang;
        $scope.getFlag = getFlag;


        function setLang (lang) {
            switch (lang) {
                case 'English':
                    $translate.use('en');
                    break;
                case 'Español':
                    $translate.use('es');
                    break;
                case '中文':
                    $translate.use('zh');
                    break;
                case '日本語':
                    $translate.use('ja');
                    break;
                case 'Portugal':
                    $translate.use('pt');
                    break;
                case 'Русский язык':
                    $translate.use('ru');
                    break;
            }
            return $scope.lang = lang;
        };

        function getFlag() {
            var lang;
            lang = $scope.lang;
            switch (lang) {
                case 'English':
                    return 'flags-american';
                    break;
                case 'Español':
                    return 'flags-spain';
                    break;
                case '中文':
                    return 'flags-china';
                    break;
                case 'Portugal':
                    return 'flags-portugal';
                    break;
                case '日本語':
                    return 'flags-japan';
                    break;
                case 'Русский язык':
                    return 'flags-russia';
                    break;
            }
        };

    }

})(); 
*/