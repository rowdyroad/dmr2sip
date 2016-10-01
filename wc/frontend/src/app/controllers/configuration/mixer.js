
'use strict';

angular
.module('ac.controllers', ['ngRoute'])
.config(['$routeProvider', function($routeProvider) {
  $routeProvider.when('/configuration/mixer', {
    templateUrl: '/views/configuration/mixer/cards.html',
    controller: 'SoundCardsCtrl'
  })
.when('/configuration/mixer/:device_id', {
    templateUrl: '/views/configuration/mixer/card.form.html',
    controller: 'SoundCardFormCtrl'
  })
}])
.controller('SoundCardFormCtrl', function($scope, $http,$routeParams, $uibModal,auth) {
  auth.Protect();
  FormController('/api/configuration/mixer', 'device_id', $scope, $http, $routeParams, $uibModal);
})
.controller('SoundCardsCtrl', function($scope, $http,auth) {
  auth.Protect();
  BaseController('/api/configuration/mixer', 'device_id', $scope, $http);
});