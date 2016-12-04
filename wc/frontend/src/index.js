import React from 'react'
import {createStore, applyMiddleware,combineReducers} from 'redux'
import {Provider} from 'react-redux'
import thunk from 'redux-thunk'
import {render} from 'react-dom'
import {reducer} from './reducers'
import { Router, Route,  browserHistory, IndexRoute } from 'react-router'
import App from './App'
import NotFound from './pages/NotFound'
import Login from './pages/Login'
import * as Actions from './actions'
import MainLayout from './components/MainLayout'
import Request from './utils/Request'

import { reducer as formReducer } from 'redux-form/immutable'

import {ItemFetcher, ListFetcher, Grid,Form, Delete, DeleteForm, Composer} from './components/entry'
import * as Point from './components/models/point'
import * as Rout from './components/models/route'

import Events from './pages/Events'



const store = createStore(combineReducers({main:reducer, form:formReducer}), window.devToolsExtension(), applyMiddleware(thunk));

const AuthConfig = {
  private: function(nextState, replace) {
      if (!store.getState().main.getIn(['user','success'])) {
        store.dispatch(Actions.LoginRedirect(nextState.location));
        replace('/login');
      }
  },
  publicOnly: function(nextState, replace) {
     if (store.getState().main.getIn(['user','success'])) {
        replace("/");
     }
  }
}

const Creator = (component, scope, componentProps) => {
  return (props)=> { return React.createElement(component, {...props, ...props[scope], ...componentProps}) };
}

const startApp = () => {
  render(
  <Provider store={store}>
    <Router history={browserHistory}>
      <Route path="/" component={App} name="Home">
          <Route component={MainLayout} onEnter={AuthConfig.private}>
            <IndexRoute component={Events} name="Events"/>
            <Route path="/events" name="Events" component={Events} link={{icon:'ti-direction-alt', title:'Events', 'section':'Observation'}}/>

            <Route path="/points" name="Points"
                                  component={ListFetcher({ scope:'points', apiUrl:'/api/points', pkAttribute:'point_id' })}
                                  link={{icon:'ti-desktop',title:'Points', 'section':'Integration'}}>

              <IndexRoute component={Creator(Grid, 'points', {cols:4, component:Point.Preview, newUrl:'/points/new'})}/>

              <Route path="new" name="New" component={Form(Point.Form, {scope:'point', pkAttribute:'point_id', listScope:'points', apiUrl:'/api/points', successRedirect:'/points/:point_id', closeRedirect:'/points'})}/>

              <Route path=":point_id" staticName={true} component={ItemFetcher({ scope: 'point', apiUrl:'/api/points', pkAttribute:'point_id', listUrl:'/points', listScope:'points', autoFetch:true})}>
                <IndexRoute component={Point.View} />
                <Route path="update" name="Update" component={Form(Point.Form, {scope:'point', pkAttribute:'point_id', listScope:'points', apiUrl:'/api/points', successRedirect:'/points/:point_id',  closeRedirect:'/points/:point_id'})}/>
                <Route path="delete" name="Delete" component={Delete(DeleteForm, {scope:'point', pkAttribute:'point_id', listScope:'points', apiUrl:'/api/points', successRedirect:'/points',  closeRedirect:'/points/:point_id'})}/>
              </Route>
            </Route>
            <Route path="/users" name="Users" link={{icon:'ti-user',title:'Users', 'section':'Managment'}}/>
            <Route path="/service" name="Service" link={{icon:'ti-settings',title:'Service', 'section':'Settings'}}/>
            <Route path="/mixer" name="Mixer"  link={{icon:'ti-volume',title:'Mixer', 'section':'Settings'}}/>
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