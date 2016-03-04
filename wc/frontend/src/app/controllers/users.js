'use strict';

angular
.module('ac.controllers', ['ngRoute'])
.config(function($routeProvider) {
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
})
.controller('UserFormCtrl', function($scope, $http,$routeParams, $uibModal, auth) {
auth.Protect();
   FormController('/api/users', 'user_id', $scope, $http, $routeParams, $uibModal,auth);
})
.controller('UserCtrl', function($scope, $http,auth) {
  auth.Protect();
  BaseController('/api/users', 'user_id', $scope, $http);
});