angular
  .module('ac.services',[])
  .service('auth', function($http, $location, $route, $rootScope, $window) {
      var self = this;
      this.user = null;

      function isGuest()
      {
        return this.user == null;
      }

      function login(user)
      {
          self.user = user;
          $rootScope.$broadcast("auth:login", user);
      }

      function logout()
      {
          self.user = null;
          $rootScope.$broadcast("auth:logout");
      }

      this.Login = function(username, password)
      {
        $http.post('/api/user/login', {username: username, password: password}).success(function(data) {
          login(data);
        });
      }

      this.Logout = function()
      {
        $http.post('/api/user/logout').success(function() {
          logout();
        });
      }

      $http.get('/api/user/check').success(function(data) {
        login(data);
      }).error(function() {
        logout();
      });

    }
);