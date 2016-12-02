import React, { Component, PropTypes } from 'react';
import { connect } from 'react-redux'
import PaperLayout from '../components/PaperLayout';
import * as Actions from '../actions'
import {Link} from 'react-router';
import * as UI from '../components/UIKit'
import Request from '../utils/Request'

class Login extends Component
{
  state = {
    data:{}
  }

  static contextTypes = {
    router: React.PropTypes.object.isRequired
  }

  onChange = (data) => {
    this.setState({data: {...this.state.data, ...data}});
  }

  componentDidUpdate = (prevProps) =>
  {
    if (this.props.user != prevProps.user) {
      this.context.router.replace(this.props.login_redirect ? this.props.login_redirect : "/");
    }
  }

  onSubmit = () => {
    this.props.actions.login(this.state.data);
  }

  render = () => {
    return (
            <PaperLayout>
              <UI.Form    title="Authorization"
                          error={this.state.error}
                          loading={this.state.loading}
                          onSubmit={this.onSubmit}
                          submitButton={{label:"Login", "color":"primary"}}>
                <UI.Input name="username" label="Username" type="text" onChange={this.onChange}></UI.Input>
                <UI.Input name="password" label="Password" type="password" onChange={this.onChange}></UI.Input>
              </UI.Form>
            </PaperLayout>
          );
  }
}

let mapDispatch = (dispatch) => {
  return {
    actions: {
      login: (user) => {
        dispatch(Actions.Post('user', '/api/user/login', user));
      }
    }
  }
}

export default connect(state => { return state.main.toJS(); }, mapDispatch)(Login)

