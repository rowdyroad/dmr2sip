 function AuthService($http, $location, $route, $rootScope, $window)
{
  var self = this;
  this.isGuest = true;
  this.isAdmin = false;
  this.roles = [];
  this.voyages = [];
  this.selected_voyage = null;


  var PERMISSIONS = {
    observer: {
        show_journal: true,
        show_observations:true,

        add_observation:true,
        edit_observation:true,
        delete_observation:true,
        synchronize:true
    },
    officer: {
        show_journal: true,
        show_emergencies:true,

        add_emergency:true,
        edit_emergency: true,
        delete_emergency:true,
        synchronize:true
    },
    supervisor: {
        show_journal: true,

        show_observations:true,
        show_emergencies:true,
        show_tasks_actions:true,
        show_plans:true,

        edit_observation:true,
        delete_observation:true,

        add_observation_observer:true,
        edit_observation_observer:true,
        delete_observation_observer:true,

        edit_emergency: true,
        delete_emergency:true,

        add_task_action:true,
        edit_task_action: true,
        delete_task_action:true,

        add_plan:true,
        edit_plan:true,
        delete_plan:true,

        add_plan:true,
        edit_plan:true,
        delete_plan:true,

        add_weather:true,
        edit_weather:true,
        delete_weather:true,

        show_tasks:true,
        show_emergency_events:true,
        show_roles:true,
        show_voyages:true,
        show_users:true,
        show_mammals:true,
        show_weathers:true,

        synchronize:true,
        synchronize_import:true,
        export:true,
        report:true
    },
    meteorologist: {
        show_journal: true,
        show_weathers:true,

        add_weather:true,
        edit_weather:true,
        delete_weather:true,
        synchronize:true
    }
  };

  function setPermissions()
  {
    self.roles = [];
    for (var i in self.voyages) {
      if (self.voyages[i].voyage_id == self.selected_voyage.voyage_id) {
           self.roles.push(self.voyages[i].role);
      }
    }
  }

  $rootScope.$on('voyage_selected', function(event, data) {
      self.selected_voyage = data;
      setPermissions();
  });

  function login(user, goToMain)
  {
    self.isGuest = false;
    self.isAdmin = user.is_admin;
    self.id = user.user_id;
    self.voyages = user.voyages;
    setPermissions();
    $rootScope.$broadcast("auth:user", user);
    $rootScope.hide_menu = false;
    $rootScope.hide_header = false;
    $rootScope.user_is_admin = user.is_admin;
    if (goToMain) {
      $location.path('/');
    }
  }

  function logout(goToForm)
  {
    self.isGuest = true;
    self.isAdmin = false;
    $rootScope.hide_menu = true;
    $rootScope.hide_header = true;
    if (goToForm) {
      $location.path('/login');
    }
  }

  this.CheckAccess = function(perm)
  {
      if (self.isAdmin) {
        return true;
      }

      for (var i in self.roles) {
        var role = self.roles[i];
        if (PERMISSIONS[role] && PERMISSIONS[role][perm]) {
          return true;
        }
      }
      return false;
  }

  this.Login = function(username, password) {
    $http.post('/api/user/login', {username: username, password: password}).success(function(data) {
      login(data, true);
    });
  }

  this.Logout = function()
  {
    $http.post('/api/user/logout').success(function() {
      logout(true);
    });
  }

  $http.get('/api/user/check', {params:{expand:'voyages'}}).success(function(data) {
    login(data, $location.path() == '/login');
  }).error(function() {
    logout($location.path() != '/login');
  });

}
