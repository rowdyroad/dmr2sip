import React, { Component } from 'react'

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