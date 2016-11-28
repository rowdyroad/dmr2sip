const PageLocation = (routes) => (
	routes.reduce((r, c) => (r += c.path ? c.path : ''), '').replace(/^\/+/g, "/")
)

export default PageLocation;