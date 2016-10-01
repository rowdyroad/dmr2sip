'use strict';

angular.module('ac.controllers', [])
  .controller('LangCtrl', ['$scope', '$rootScope','$translate', LangCtrl]);
    function LangCtrl($scope, $rootScope, $translate) {



        $scope.lang = 'English';
        $scope.setLang = setLang;
        $scope.getFlag = getFlag;

        function setLang (lang) {
            switch (lang) {
                default:
                case 'English':
                    $translate.use('en');
                    break;
                case 'Русский язык':
                    $translate.use('ru');
                    break;
            }
            return $scope.lang = lang;
        };

        function getFlag() {
            switch ($scope.lang) {
                default:
                case 'English':
                    return 'flag-en';
                case 'Русский язык':
                    return 'flag-ru';
            }
        };

    }