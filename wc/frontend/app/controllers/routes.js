'use strict';

angular.module('myApp.routes', ['ngRoute'])

.config(['$routeProvider', function($routeProvider) {
  $routeProvider.when('/routes', {
    templateUrl: '/views/routes/routes.html',
    controller: 'RouteCtrl'
  }).when('/routes/new', {
    templateUrl: '/views/routes/route.form.html',
    controller: 'RouteFormCtrl'
  })
.when('/routes/:route_id', {
    templateUrl: '/views/routes/route.form.html',
    controller: 'RouteFormCtrl'
  })
}])
.controller('RouteFormCtrl', function($scope, $http,$routeParams, $modal) {
  $scope.points = [];
  $http.get('/api/points').success(function(data) {
     for (var i in data) {
      data[i].name = data[i].type + ':' + data[i].id;
      $scope.points.push(data[i]);
     }
     console.log($scope.points);
  });

	FormController('/api/routes', 'route_id', $scope, $http, $routeParams, $modal);
})
.controller('RouteCtrl', function($scope, $http) {
  BaseController('/api/routes', 'route_id', $scope, $http);
});