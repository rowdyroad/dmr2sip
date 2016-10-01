'use strict';

angular
.module('ac.controllers', ['ngRoute'])
.config(['$routeProvider', function($routeProvider) {
  $routeProvider.when('/points', {
    templateUrl: '/views/points/points.html',
    controller: 'PointCtrl'
  }).when('/points/new', {
    templateUrl: '/views/points/point.form.html',
    controller: 'PointFormCtrl'
  })
.when('/points/:point_id', {
    templateUrl: '/views/points/point.form.html',
    controller: 'PointFormCtrl'
  })
}])
.controller('PointFormCtrl', function($scope, $http,$routeParams, $uibModal,auth) {
  auth.Protect();
  $scope.types = [{id:'sip', name:'SIP'}, {id:'dmr', name:'DMR'}];
  FormController('/api/points', 'point_id', $scope, $http, $routeParams, $uibModal);
})
.controller('PointCtrl', function($scope, $http,auth) {
  auth.Protect();
  BaseController('/api/points', 'point_id', $scope, $http);
  $scope.$on('points:states', function(e, data) {
    angular.forEach(data, function(state) {
        angular.forEach($scope.items, function(point) {
          if (point.point_id == state.point_id) {
            point.status = state.status;
          }
        });
    });
  });
});