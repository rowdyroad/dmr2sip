'use strict';

angular
.module('ac.controllers', ['ngRoute'])
.config(function($routeProvider) {
  $routeProvider.when('/categories', {
    templateUrl: '/views/categories/categories.html',
    controller: 'CategoryCtrl'
  }).when('/categories/new', {
    templateUrl: '/views/categories/category.form.html',
    controller: 'CategoryFormCtrl'
  })
.when('/categories/:category_id', {
    templateUrl: '/views/categories/category.form.html',
    controller: 'CategoryFormCtrl'
  })
})
.controller('CategoryFormCtrl', function($scope, $http,$routeParams, $uibModal) {
	FormController('/api/course/categories', 'category_id', $scope, $http, $routeParams, $uibModal);
})
.controller('CategoryCtrl', function($scope, $http) {
  BaseController('/api/course/categories', 'category_id', $scope, $http);
});