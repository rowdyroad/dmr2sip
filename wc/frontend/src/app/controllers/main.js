'use strict'

angular
.module('ac.controllers', [])
.controller('MainCtrl', function($scope, auth) {
    $scope.user = null;
    $scope.loaded = false;

    $scope.$on('auth:login', function(event, user) {
      $scope.user = user;
      $scope.loaded = true;
    });

    $scope.$on('auth:logout', function(event) {
      $scope.user = null;
      $scope.loaded = true;
    });
});