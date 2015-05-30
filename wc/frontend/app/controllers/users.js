'use strict';

angular.module('myApp.users', ['ngRoute'])

.config(['$routeProvider', function($routeProvider) {
  $routeProvider.when('/users', {
    templateUrl: '/views/users/users.html',
    controller: 'UserCtrl'
  }).when('/users/new', {
    templateUrl: '/views/users/user.form.html',
    controller: 'UserFormCtrl'
  })
.when('/users/:user_id', {
    templateUrl: '/views/users/user.form.html',
    controller: 'UserFormCtrl'
  })
}])
.controller('UserFormCtrl', function($scope, $http,$routeParams, $modal) {
	FormController('/api/users', 'user_id', $scope, $http, $routeParams, $modal);
})
.controller('UserCtrl', function($scope, $http) {
  BaseController('/api/users', 'user_id', $scope, $http);
});