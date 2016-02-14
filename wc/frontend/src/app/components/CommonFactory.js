'use strict'

var CommonFactory = {

	create: function(config)
	{
		config.path = pluralize(config.type);
		config.pkAttribute = config.type + '_id';

		angular.module('ac.controllers', ['ngRoute'])
			.config(function($routeProvider) {
			  $routeProvider.when('/' + config.path, {
			    templateUrl: '/views/list.html',
			    controller: config.type + 'Ctrl'
			  }).when('/' + config.path + '/new', {
			    templateUrl: '/views/users/user.form.html',
			    controller: config.type + 'FormCtrl'
			  })
			.when('/' + config.path + '/:' + config.pkAttribute, {
			    templateUrl: '/views/users/user.form.html',
			    controller: config.type + 'FormCtrl'
			  })
			})
			.controller(config.type + 'FormCtrl', function($scope, $http,$routeParams, $modal) {
			  $scope.config = config;
			  FormController('/api/' + config.path, config.pkAttribute, $scope, $http, $routeParams, $modal);
			})
			.controller(config.type + 'Ctrl', function($scope, $http) {
			  $scope.config = config;
			  BaseController('/api/' + config.path, config.pkAttribute, $scope, $http);
			});

	}
};