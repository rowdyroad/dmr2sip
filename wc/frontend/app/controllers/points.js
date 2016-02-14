'use strict';

angular.module('myApp.points', ['ngRoute'])

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
.controller('PointFormCtrl', function($scope, $http,$routeParams, $uibModal) {
  $scope.types = [{id:'sip', name:'SIP'}, {id:'dmr', name:'DMR'}];
	FormController('/api/points', 'point_id', $scope, $http, $routeParams, $uibModal);
})
.controller('PointCtrl', function($scope, $http) {
  BaseController('/api/points', 'point_id', $scope, $http);
});