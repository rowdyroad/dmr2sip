import React, { Component, PropTypes } from 'react';
import { connect } from 'react-redux'
import logo from '../images/logo.png';

class CenterLayout extends Component
{
	render = () => {
		return (<div>
              <div className="account-pages"></div>
              <div className="clearfix"></div>
                <div className="wrapper-page">
                  {this.props.children}
                </div>
            </div>);
	}
}

export default CenterLayout;