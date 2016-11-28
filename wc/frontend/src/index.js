import React from 'react';
import {createStore, applyMiddleware,combineReducers} from 'redux';
import {Provider} from 'react-redux';
import thunk from 'redux-thunk'
import {render} from 'react-dom';
import {reducer} from './reducers';
import { Router, Route,  browserHistory, IndexRoute } from 'react-router';
import App from './App';
import NotFound from './pages/NotFound';
import Login from './pages/Login';
import * as Actions from './actions';
import MainLayout from './components/MainLayout';
import {loginRedirect} from './actions';
import Request from './utils/Request';

import { reducer as formReducer } from 'redux-form/immutable'


import Events from './pages/Events';
import Points from './pages/point/List';
import Point from './pages/point/Item';

import PointView from './pages/point/View';
import PointUpdateForm from './pages/point/UpdateForm';
import PointCreateForm from './pages/point/CreateForm';
import Routes from './pages/Routes';
import Users from './pages/Users';


const store = createStore(combineReducers({main:reducer, form:formReducer}), window.devToolsExtension(), applyMiddleware(thunk));

const AuthConfig = {
  private: function(nextState, replace) {
      if (!store.getState().main.getIn(['user','success'])) {
        store.dispatch(Actions.LoginRedirect(nextState.location));
        replace('/login');
      }
  },
  publicOnly: function(nextState, replace) {
     if (store.getState().getIn(['user','success'])) {
        replace("/");
     }
  }
}

const startApp = () => {
  render(
  <Provider store={store}>
    <Router history={browserHistory}>
      <Route path="/" component={App} name="Home">
          <Route component={MainLayout} onEnter={AuthConfig.private}>
            <IndexRoute component={Events} name="Events"/>
            <Route path="/events" name="Events" component={Events} link={{icon:'ti-direction-alt', title:'Events', 'section':'Observation'}}/>
            <Route path="/points" name="Points" link={{icon:'ti-desktop',title:'Points', 'section':'Integration'}}>
              <IndexRoute component={Points} />
              <Route path="new" name="New" component={PointCreateForm}/>
              <Route path=":point_id" staticName={true} component={Point}>
                <IndexRoute component={PointView} />
                <Route path="edit" name="Update" component={PointUpdateForm}/>
              </Route>
            </Route>
            <Route path="/routes" name="Routes" component={Routes} link={{icon:'ti-back-right',title:'Routes', 'section':'Integration'}}/>
            <Route path="/users" name="Users" component={Users} link={{icon:'ti-user',title:'Users', 'section':'Managment'}}/>
            <Route path="/service" name="Service" component={Users} link={{icon:'ti-settings',title:'Service', 'section':'Settings'}}/>
            <Route path="/mixer" name="Mixer"  component={Users} link={{icon:'ti-volume',title:'Mixer', 'section':'Settings'}}/>
          </Route>
          <Route path="/login" component={Login} onEnter={AuthConfig.publicOnly}/>
      </Route>
      <Route path="*" component={NotFound}/>
    </Router>
  </Provider>,
    document.getElementById('root')
  );
}

Request.get('/api/user/check').then((data) => {
  store.dispatch(Actions.RequestSuccess('user', data));
  startApp();
})
.catch(startApp);