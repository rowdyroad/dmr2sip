'use strict';

angular.module('ac.controllers', ['ngRoute'])
.config(function($routeProvider) {
  $routeProvider.when('/configuration/network', {
    templateUrl: '/views/configuration/network.html',
    controller: 'ConfigurationNetworkCtrl'
  });
})
.controller('ConfigurationNetworkCtrl', function($scope, $http, auth) {
    auth.Protect();
    $http.get('/api/configuration/network').success(function(data) {
        $scope.current = data;
    });

    $scope.types = ['static','dhcp'];

    $scope.save = function()
    {
         $http.post('/api/configuration/network', $scope.current);
    }
});